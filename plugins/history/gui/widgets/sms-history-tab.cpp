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

	setUpGui();
}

SmsHistoryTab::~SmsHistoryTab()
{
}

void SmsHistoryTab::setUpGui()
{
	SmsBuddiesModel = new BuddyListModel(talkableTree());
	ModelChain *chain = new ModelChain(SmsBuddiesModel, talkableTree());

	TalkableProxyModel *proxyModel = new TalkableProxyModel(chain);
	proxyModel->setSortByStatusAndUnreadMessages(false);

	NameTalkableFilter *nameTalkableFilter = new NameTalkableFilter(NameTalkableFilter::AcceptMatching, proxyModel);
	connect(filteredView(), SIGNAL(filterChanged(QString)), nameTalkableFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameTalkableFilter);

	chain->addProxyModel(proxyModel);

	talkableTree()->setChain(chain);

	connect(talkableTree(), SIGNAL(currentChanged(Talkable)), this, SLOT(currentSmsChanged(Talkable)));
	connect(talkableTree(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSmsPopupMenu()));
	talkableTree()->setContextMenuPolicy(Qt::CustomContextMenu);
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

	BuddySet buddies = talkableTree()->actionContext()->buddies();
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

	QVector<Talkable> talkables = SmsFutureWatcher->result();

	BuddyList buddies;

	foreach (const Talkable &talkable, talkables)
		if (talkable.isValidBuddy())
			buddies.append(talkable.toBuddy());

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

	QFuture<QVector<Talkable> > futureSms = historyStorage()->smsRecipients();
	SmsFutureWatcher = new QFutureWatcher<QVector<Talkable> >(this);
	connect(SmsFutureWatcher, SIGNAL(finished()), this, SLOT(futureSmsAvailable()));
	connect(SmsFutureWatcher, SIGNAL(canceled()), this, SLOT(futureSmsCanceled()));

	SmsFutureWatcher->setFuture(futureSms);

	showTabWaitOverlay();
}
