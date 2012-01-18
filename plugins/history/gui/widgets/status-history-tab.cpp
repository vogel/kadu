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
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-tree-view.h"
#include "model/model-chain.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "model/dates-model-item.h"
#include "model/history-dates-model.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "search/history-search-parameters.h"
#include "history.h"
#include "timed-status.h"

#include "status-history-tab.h"

StatusHistoryTab::StatusHistoryTab(QWidget *parent) :
		HistoryTab(parent)
{
	MyBuddyStatusDatesModel = new HistoryDatesModel(false, this);

	createGui();
}

StatusHistoryTab::~StatusHistoryTab()
{
}

void StatusHistoryTab::createTreeView(QWidget *parent)
{
	FilteredTreeView *statusesTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);
	statusesTalkableWidget->setFilterAutoVisibility(false);

	StatusesTalkableTree = new TalkableTreeView(statusesTalkableWidget);
	StatusesTalkableTree->setUseConfigurationColors(true);
	StatusesTalkableTree->setContextMenuEnabled(true);
	StatusesTalkableTree->delegateConfiguration().setShowMessagePixmap(false);

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
	connect(StatusesTalkableTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showStatusesPopupMenu(QPoint)));
	StatusesTalkableTree->setContextMenuPolicy(Qt::CustomContextMenu);

	statusesTalkableWidget->setView(StatusesTalkableTree);
}

void StatusHistoryTab::updateData()
{
	QVector<Buddy> statusBuddies = History::instance()->statusBuddiesList(HistorySearchParameters());
	StatusBuddiesModel->setBuddyList(statusBuddies.toList());
}

void StatusHistoryTab::statusBuddyActivated(const Buddy &buddy)
{
	QModelIndex selectedIndex = timelineView()->timeline()->model()
	        ? timelineView()->timeline()->selectionModel()->currentIndex()
	        : QModelIndex();

	QDate date = selectedIndex.data(DateRole).toDate();

	QVector<DatesModelItem> statusDates = History::instance()->datesForStatusBuddy(buddy, HistorySearchParameters());
	MyBuddyStatusDatesModel->setDates(statusDates);

	if (date.isValid())
		selectedIndex = MyBuddyStatusDatesModel->indexForDate(date);
	if (!selectedIndex.isValid())
	{
		int lastRow = MyBuddyStatusDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			selectedIndex = MyBuddyStatusDatesModel->index(lastRow);
	}

	timelineView()->timeline()->setModel(MyBuddyStatusDatesModel);

	connect(timelineView()->timeline()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(statusDateCurrentChanged(QModelIndex,QModelIndex)), Qt::UniqueConnection);

	timelineView()->timeline()->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void StatusHistoryTab::statusDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (current == previous)
		return;

	QDate date = current.data(DateRole).value<QDate>();

	timelineView()->messagesView()->setUpdatesEnabled(false);

	timelineView()->messagesView()->clearMessages();

	if (!StatusesTalkableTree->actionContext()->buddies().isEmpty())
	{
		Buddy buddy = *StatusesTalkableTree->actionContext()->buddies().begin();
		QList<TimedStatus> statuses;
		if (buddy && date.isValid())
			statuses = History::instance()->statuses(buddy, date);
		if (!buddy.contacts().isEmpty())
			timelineView()->messagesView()->setChat(ChatManager::instance()->findChat(ContactSet(buddy.contacts().at(0)), true));
		timelineView()->messagesView()->appendMessages(statusesToMessages(statuses));
	}

	timelineView()->messagesView()->setUpdatesEnabled(true);
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
}

void StatusHistoryTab::removeEntriesPerDate(const QDate &date)
{
	if (!StatusesTalkableTree->actionContext()->buddies().isEmpty())
	{
		Buddy buddy = *StatusesTalkableTree->actionContext()->buddies().begin();
		if (buddy && !buddy.contacts().isEmpty())
		{
			History::instance()->currentStorage()->clearStatusHistory(buddy, date);
			statusBuddyActivated(buddy);
		}
	}
}

void StatusHistoryTab::currentStatusChanged(const Talkable &talkable)
{
	statusBuddyActivated(talkable.toBuddy());
}

void StatusHistoryTab::showStatusesPopupMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	QScopedPointer<QMenu> menu;

	menu.reset(TalkableMenuManager::instance()->menu(this, StatusesTalkableTree->actionContext()));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Status History"), this, SLOT(clearStatusHistory()));

	menu->exec(QCursor::pos());
}
