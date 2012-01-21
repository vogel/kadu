/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QMenu>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "buddies/model/buddy-list-model.h"
#include "chat/chat-manager.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-tree-view.h"
#include "model/model-chain.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "model/dates-model-item.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "search/history-search-parameters.h"
#include "history.h"
#include "timed-status.h"

#include "status-history-tab.h"

StatusHistoryTab::StatusHistoryTab(QWidget *parent) :
		HistoryTab(false, parent), IsBuddy(true)
{
	createGui();
}

StatusHistoryTab::~StatusHistoryTab()
{
}

void StatusHistoryTab::createTreeView(QWidget *parent)
{
	FilteredTreeView *statusesTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);
	statusesTalkableWidget->filterWidget()->setAutoVisibility(false);
	statusesTalkableWidget->filterWidget()->setLabel(tr("Filter") + ":");

	StatusesTalkableTree = new TalkableTreeView(statusesTalkableWidget);
	StatusesTalkableTree->setAlternatingRowColors(true);
	StatusesTalkableTree->setContextMenuEnabled(true);
	StatusesTalkableTree->setUseConfigurationColors(true);
	StatusesTalkableTree->delegateConfiguration().setShowMessagePixmap(false);

	QString style;
	style.append("QTreeView::branch:has-siblings:!adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-siblings:adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-childres:!has-siblings:adjoins-item { border-image: none; image: none }");
	StatusesTalkableTree->setStyleSheet(style);
	StatusesTalkableTree->viewport()->setStyleSheet(style);

	StatusBuddiesModel = new BuddyListModel(StatusesTalkableTree);
	StatusesModelChain = new ModelChain(StatusBuddiesModel, StatusesTalkableTree);

	TalkableProxyModel *proxyModel = new TalkableProxyModel(StatusesModelChain);
	proxyModel->setSortByStatusAndUnreadMessages(false);

	NameTalkableFilter *nameTalkableFilter = new NameTalkableFilter(NameTalkableFilter::AcceptMatching, proxyModel);
	connect(statusesTalkableWidget, SIGNAL(filterChanged(QString)), nameTalkableFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameTalkableFilter);

	StatusesModelChain->addProxyModel(proxyModel);

	StatusesTalkableTree->setChain(StatusesModelChain);

	connect(StatusesTalkableTree, SIGNAL(currentChanged(Talkable)), this, SLOT(currentStatusChanged(Talkable)));
	connect(StatusesTalkableTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showStatusesPopupMenu()));
	StatusesTalkableTree->setContextMenuPolicy(Qt::CustomContextMenu);

	statusesTalkableWidget->setView(StatusesTalkableTree);
}

void StatusHistoryTab::updateData()
{
	QVector<Buddy> statusBuddies = History::instance()->statusBuddiesList();
	StatusBuddiesModel->setBuddyList(statusBuddies.toList());
}

void StatusHistoryTab::displayStatusBuddy(const Buddy &buddy, bool force)
{
	if (!force && CurrentBuddy == buddy && IsBuddy)
		return;

	IsBuddy = true;

	BuddySet buddies;
	buddies.insert(buddy);
	timelineView()->messagesView()->setChat(ChatManager::instance()->findChat(buddies, true));

	CurrentBuddy = buddy;
	setDates(History::instance()->datesForStatusBuddy(CurrentBuddy));
}

void StatusHistoryTab::displayStatusContact(const Contact &contact, bool force)
{
	if (!force && CurrentContact == contact && !IsBuddy)
		return;

	IsBuddy = false;

	ContactSet contacts;
	contacts.insert(contact);
	timelineView()->messagesView()->setChat(ChatManager::instance()->findChat(contacts, true));

	CurrentContact = contact;
	setDates(History::instance()->datesForStatusContact(CurrentContact));
}

QVector<Message> StatusHistoryTab::statusesToMessages(const QList<TimedStatus> &statuses)
{
	QVector<Message> messages;

	foreach (const TimedStatus &timedStatus, statuses)
	{
		Message message = Message::create();
		message.setStatus(MessageStatusReceived);
		message.setType(MessageTypeReceived);

		const StatusTypeData &typeData = StatusTypeManager::instance()->statusTypeData(timedStatus.status().type());

		if (timedStatus.status().description().isEmpty())
			message.setContent(typeData.name());
		else
			message.setContent(QString("%1 with description: %2")
					.arg(typeData.name())
					.arg(timedStatus.status().description()));

		message.setReceiveDate(timedStatus.dateTime());
		message.setSendDate(timedStatus.dateTime());

		messages.append(message);
	}

	return messages;
}

void StatusHistoryTab::showStatusesPopupMenu()
{
	QScopedPointer<QMenu> menu;

	menu.reset(TalkableMenuManager::instance()->menu(this, StatusesTalkableTree->actionContext()));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Status History"), this, SLOT(clearStatusHistory()));

	menu->exec(QCursor::pos());
}

void StatusHistoryTab::clearStatusHistory()
{
	if (!StatusesTalkableTree->actionContext())
		return;

	const BuddySet &buddies = StatusesTalkableTree->actionContext()->buddies();
	if (buddies.isEmpty())
		return;

	foreach (const Buddy &buddy, buddies)
		History::instance()->currentStorage()->clearStatusHistory(buddy);

	updateData();
	displayStatusBuddy(Buddy::null, false);
}

void StatusHistoryTab::displayForDate(const QDate &date)
{
	timelineView()->messagesView()->setUpdatesEnabled(false);
	timelineView()->messagesView()->clearMessages();

	QList<TimedStatus> statuses;
	if (IsBuddy)
	{
		if (CurrentBuddy && date.isValid())
			statuses = History::instance()->statuses(CurrentBuddy, date);
		if (!CurrentBuddy.contacts().isEmpty())
			timelineView()->messagesView()->setChat(ChatManager::instance()->findChat(ContactSet(CurrentBuddy.contacts().at(0)), true));
	}
	else
	{
		if (CurrentContact && date.isValid())
			statuses = History::instance()->statuses(CurrentContact, date);
		timelineView()->messagesView()->setChat(ChatManager::instance()->findChat(ContactSet(CurrentContact), true));
	}
	timelineView()->messagesView()->appendMessages(statusesToMessages(statuses));
	timelineView()->messagesView()->refresh();

	timelineView()->messagesView()->setUpdatesEnabled(true);
}

void StatusHistoryTab::removeEntriesPerDate(const QDate &date)
{
	if (CurrentBuddy)
	{
		History::instance()->currentStorage()->clearStatusHistory(CurrentBuddy, date);
		displayStatusBuddy(CurrentBuddy, true);
	}
}

void StatusHistoryTab::currentStatusChanged(const Talkable &talkable)
{
	switch (talkable.type())
	{
		case Talkable::ItemBuddy:
			displayStatusBuddy(talkable.toBuddy(), false);
			break;
		case Talkable::ItemContact:
			displayStatusContact(talkable.toContact(), false);
			break;
		default:
			displayStatusBuddy(Buddy::null, false);
			break;
	}
}
