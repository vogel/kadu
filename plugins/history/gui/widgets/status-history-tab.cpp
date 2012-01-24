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
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "icons/kadu-icon.h"
#include "model/model-chain.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "model/dates-model-item.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "search/history-search-parameters.h"
#include "storage/history-storage.h"

#include "status-history-tab.h"

StatusHistoryTab::StatusHistoryTab(QWidget *parent) :
		HistoryTab(false, parent), IsBuddy(true)
{
	setUpGui();
}

StatusHistoryTab::~StatusHistoryTab()
{
}

void StatusHistoryTab::setUpGui()
{
	connect(talkableTree(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showStatusesPopupMenu()));
	talkableTree()->setContextMenuPolicy(Qt::CustomContextMenu);
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

	if (historyStorage())
		setFutureDates(historyStorage()->statusDates(CurrentBuddy));
	else
		setDates(QVector<DatesModelItem>());
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

	if (historyStorage())
		setFutureDates(historyStorage()->statusDates(CurrentContact));
	else
		setDates(QVector<DatesModelItem>());
}

void StatusHistoryTab::showStatusesPopupMenu()
{
	QScopedPointer<QMenu> menu;

	menu.reset(TalkableMenuManager::instance()->menu(this, talkableTree()->actionContext()));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear Status History"), this, SLOT(clearStatusHistory()));

	menu->exec(QCursor::pos());
}

void StatusHistoryTab::clearStatusHistory()
{
	if (!talkableTree()->actionContext())
		return;

	if (!historyStorage())
		return;

	const BuddySet &buddies = talkableTree()->actionContext()->buddies();
	if (buddies.isEmpty())
		return;

	foreach (const Buddy &buddy, buddies)
		historyStorage()->clearStatusHistory(buddy);

	updateData();
	displayStatusBuddy(Buddy::null, false);
}

void StatusHistoryTab::displayForDate(const QDate &date)
{
	if (!historyStorage() || (IsBuddy && !CurrentBuddy) || (!IsBuddy && !CurrentContact) || !date.isValid())
	{
		setMessages(QVector<Message>());
		return;
	}

	if (IsBuddy)
	{
		if (!CurrentBuddy.contacts().isEmpty())
			timelineView()->messagesView()->setChat(ChatManager::instance()->findChat(ContactSet(CurrentBuddy.contacts().at(0)), true));
		setFutureMessages(historyStorage()->statuses(CurrentBuddy, date));
	}
	else
	{
		timelineView()->messagesView()->setChat(ChatManager::instance()->findChat(ContactSet(CurrentContact), true));
		setFutureMessages(historyStorage()->statuses(CurrentContact, date));
	}
}

void StatusHistoryTab::removeEntriesPerDate(const QDate &date)
{
	if (CurrentBuddy && historyStorage())
	{
		historyStorage()->clearStatusHistory(CurrentBuddy, date);
		displayStatusBuddy(CurrentBuddy, true);
	}
}

void StatusHistoryTab::currentTalkableChanged(const Talkable &talkable)
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

void StatusHistoryTab::updateData()
{
	setMessages(QVector<Message>());

	if (!historyStorage())
	{
		setTalkables(QVector<Talkable>());
		displayStatusBuddy(Buddy::null, false);
		return;
	}

	setFutureTalkables(historyStorage()->statusBuddies());
}
