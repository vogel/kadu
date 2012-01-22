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
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"
#include "model/model-chain.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "talkable/talkable.h"

#include "model/dates-model-item.h"
#include "search/history-search-parameters.h"
#include "storage/history-storage.h"

#include "sms-history-tab.h"

SmsHistoryTab::SmsHistoryTab(QWidget *parent) :
		HistoryTab(false, parent), SmsFutureWatcher(0)
{
	CurrentRecipient = Buddy::create();

	createGui();
}

SmsHistoryTab::~SmsHistoryTab()
{
}

void SmsHistoryTab::createTreeView(QWidget *parent)
{
	FilteredTreeView *smsTalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);
	smsTalkableWidget->filterWidget()->setAutoVisibility(false);
	smsTalkableWidget->filterWidget()->setLabel(tr("Filter") + ":");

	SmsTalkableTree = new TalkableTreeView(smsTalkableWidget);
	SmsTalkableTree->setAlternatingRowColors(true);
	SmsTalkableTree->setContextMenuEnabled(true);
	SmsTalkableTree->setUseConfigurationColors(true);
	SmsTalkableTree->delegateConfiguration().setShowMessagePixmap(false);

	QString style;
	style.append("QTreeView::branch:has-siblings:!adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-siblings:adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-childres:!has-siblings:adjoins-item { border-image: none; image: none }");
	SmsTalkableTree->setStyleSheet(style);
	SmsTalkableTree->viewport()->setStyleSheet(style);

	SmsBuddiesModel = new BuddyListModel(SmsTalkableTree);
	SmsModelChain = new ModelChain(SmsBuddiesModel, SmsTalkableTree);

	TalkableProxyModel *proxyModel = new TalkableProxyModel(SmsModelChain);
	proxyModel->setSortByStatusAndUnreadMessages(false);

	NameTalkableFilter *nameTalkableFilter = new NameTalkableFilter(NameTalkableFilter::AcceptMatching, proxyModel);
	connect(smsTalkableWidget, SIGNAL(filterChanged(QString)), nameTalkableFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameTalkableFilter);

	SmsModelChain->addProxyModel(proxyModel);

	SmsTalkableTree->setChain(SmsModelChain);

	connect(SmsTalkableTree, SIGNAL(currentChanged(Talkable)), this, SLOT(currentSmsChanged(Talkable)));
	connect(SmsTalkableTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSmsPopupMenu()));
	SmsTalkableTree->setContextMenuPolicy(Qt::CustomContextMenu);

	smsTalkableWidget->setView(SmsTalkableTree);
}

void SmsHistoryTab::displaySmsRecipient(const QString& recipient, bool force)
{
	if (!force && CurrentRecipient.mobile() == recipient)
		return;

	Chat smsChat = Chat::create();
	smsChat.setDisplay(recipient);
	timelineView()->messagesView()->setChat(smsChat);

	CurrentRecipient.setMobile(recipient);

	if (historyStorage())
		setFutureDates(historyStorage()->smsRecipientDates(Talkable(CurrentRecipient)));
	else
		setDates(QVector<DatesModelItem>());
}

void SmsHistoryTab::showSmsPopupMenu()
{
	QScopedPointer<QMenu> menu;

	menu.reset(new QMenu(this));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear SMS History"), this, SLOT(clearSmsHistory()));

	menu->exec(QCursor::pos());
}

void SmsHistoryTab::clearSmsHistory()
{
	bool removed = false;

	if (!historyStorage())
		return;

	BuddySet buddies = SmsTalkableTree->actionContext()->buddies();
	foreach (const Buddy &buddy, buddies)
	{
		if (buddy.mobile().isEmpty())
			continue;

		removed = true;
		historyStorage()->clearSmsHistory(buddy);
	}

	if (removed)
	{
		updateData();
		displaySmsRecipient(QString(), false);
	}
}

void SmsHistoryTab::currentSmsChanged(const Talkable &talkable)
{
	if (talkable.isValidBuddy())
		displaySmsRecipient(talkable.toBuddy().mobile(), false);
	else
		displaySmsRecipient(QString(), false);
}

void SmsHistoryTab::displayForDate(const QDate &date)
{
	if (!CurrentRecipient.mobile().isEmpty() && date.isValid() && historyStorage())
		setFutureMessages(historyStorage()->smses(CurrentRecipient, date));
	else
		setMessages(QVector<Message>());
}

void SmsHistoryTab::removeEntriesPerDate(const QDate &date)
{
	if (!CurrentRecipient.mobile().isEmpty() && historyStorage())
	{
		historyStorage()->clearSmsHistory(CurrentRecipient, date);
		displaySmsRecipient(CurrentRecipient.mobile(), true);
	}
}

void SmsHistoryTab::futureSmsAvailable()
{
	hideTabWaitOverlay();

	if (!SmsFutureWatcher)
		return;

	QVector<QString> smsRecipients = SmsFutureWatcher->result();

	BuddyList buddies;

	foreach (const QString &smsRecipient, smsRecipients)
	{
		Buddy buddy = Buddy::create();
		buddy.setDisplay(smsRecipient);
		buddy.setMobile(smsRecipient);
		buddies.append(buddy);
	}

	SmsBuddiesModel->setBuddyList(buddies);

	SmsFutureWatcher->deleteLater();
	SmsFutureWatcher = 0;
}

void SmsHistoryTab::futureSmsCanceled()
{
	hideTabWaitOverlay();

	if (!SmsFutureWatcher)
		return;

	SmsFutureWatcher->deleteLater();
	SmsFutureWatcher = 0;
}

void SmsHistoryTab::updateData()
{
	setMessages(QVector<Message>());

	if (SmsFutureWatcher)
	{
		SmsFutureWatcher->cancel();
		SmsFutureWatcher->deleteLater();
	}

	if (!historyStorage())
	{
		SmsBuddiesModel->setBuddyList(BuddyList());
		displaySmsRecipient(QString(), false);
		return;
	}

	QFuture<QVector<QString> > futureSms = historyStorage()->smsRecipients();
	SmsFutureWatcher = new QFutureWatcher<QVector<QString> >(this);
	connect(SmsFutureWatcher, SIGNAL(finished()), this, SLOT(futureSmsAvailable()));
	connect(SmsFutureWatcher, SIGNAL(canceled()), this, SLOT(futureSmsCanceled()));

	SmsFutureWatcher->setFuture(futureSms);

	showTabWaitOverlay();
}
