/*
 * %kadu copyright begin%
 * %kadu copyright end%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "chat/model/chat-data-extractor.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "message/message-manager.h"
#include "model/roles.h"
#include "status/status-type.h"
#include "status/status.h"
#include "talkable/filter/talkable-filter.h"

#include "talkable-proxy-model.h"

TalkableProxyModel::TalkableProxyModel(QObject *parent) :
		QSortFilterProxyModel(parent), SortByStatusAndUnreadMessages(true)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");

	connect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
			this, SLOT(invalidate()));
	connect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
			this, SLOT(invalidate()));

	connect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
			this, SIGNAL(invalidated()));
	connect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
			this, SIGNAL(invalidated()));
}

TalkableProxyModel::~TalkableProxyModel()
{
}

int TalkableProxyModel::compareNames(const QString &n1, const QString &n2) const
{
	return BrokenStringCompare
			? n1.toLower().localeAwareCompare(n2.toLower())
			: n1.localeAwareCompare(n2);
}

void TalkableProxyModel::setSortByStatusAndUnreadMessages(bool sortByStatusAndUnreadMessages)
{
	if (SortByStatusAndUnreadMessages == sortByStatusAndUnreadMessages)
		return;

	SortByStatusAndUnreadMessages = sortByStatusAndUnreadMessages;
	invalidate();
}

bool TalkableProxyModel::lessThan(const Chat &left, const Chat &right) const
{
	if (left.unreadMessagesCount() > 0 && right.unreadMessagesCount() == 0)
		return -1;
	if (left.unreadMessagesCount() == 0 && right.unreadMessagesCount() > 0)
		return 1;

	const QString &leftChatDisplay = ChatDataExtractor::data(left, Qt::DisplayRole).toString();
	const QString &rightChatDisplay = ChatDataExtractor::data(right, Qt::DisplayRole).toString();

	int displayCompare = compareNames(leftChatDisplay, rightChatDisplay);
	return displayCompare < 0;
}

bool TalkableProxyModel::lessThan(const Buddy &left, const Buddy &right) const
{
	if (left.contacts().isEmpty() && !right.contacts().isEmpty())
		return false;
	if (!left.contacts().isEmpty() && right.contacts().isEmpty())
		return true;

	if (left.isBlocked() && !right.isBlocked())
		return false;
	if (!left.isBlocked() && right.isBlocked())
		return true;

	if (SortByStatusAndUnreadMessages)
	{
		const bool leftHasUnreadMessages = left.unreadMessagesCount() > 0;
		const bool rightHasUnreadMessages = right.unreadMessagesCount() > 0;
		if (!leftHasUnreadMessages && rightHasUnreadMessages)
			return false;
		if (leftHasUnreadMessages && !rightHasUnreadMessages)
			return true;

		const Contact &leftContact = BuddyPreferredManager::instance()->preferredContact(left, false);
		const Contact &rightContact = BuddyPreferredManager::instance()->preferredContact(right, false);

		if (leftContact.isBlocking() && !rightContact.isBlocking())
			return false;

		if (!leftContact.isBlocking() && rightContact.isBlocking())
			return true;

		const Status &leftStatus = !leftContact.isNull()
				? leftContact.currentStatus()
				: Status();
		const Status &rightStatus = !rightContact.isNull()
				? rightContact.currentStatus()
				: Status();

		if (leftStatus.isDisconnected() && !rightStatus.isDisconnected())
			return false;

		if (!leftStatus.isDisconnected() && rightStatus.isDisconnected())
			return true;
	}

	const int displayCompare = compareNames(left.display(), right.display());
	return displayCompare < 0;
}

bool TalkableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	const int leftRole = left.data(ItemTypeRole).value<int>();
	const int rightRole = right.data(ItemTypeRole).value<int>();

	// first - chats
	if (ChatRole == leftRole && ChatRole != rightRole)
		return true;
	if (ChatRole != leftRole && ChatRole == rightRole)
		return false;

	// second - buddies
	if (BuddyRole == leftRole && BuddyRole != rightRole)
		return true;
	if (BuddyRole != leftRole && BuddyRole == rightRole)
		return false;

	// if not, we have a problem...
	Q_ASSERT(leftRole == rightRole);

	switch (leftRole)
	{
		case ChatRole:
			return lessThan(left.data(ChatRole).value<Chat>(), right.data(ChatRole).value<Chat>());
		case BuddyRole:
			return lessThan(left.data(BuddyRole).value<Buddy>(), right.data(BuddyRole).value<Buddy>());
		case ContactRole:
			return lessThan(left.data(ContactRole).value<Contact>().ownerBuddy(), right.data(ContactRole).value<Contact>().ownerBuddy());
		default:
			Q_ASSERT(false);
	}

	return false;
}

bool TalkableProxyModel::accept(const Chat &chat) const
{
	foreach (TalkableFilter *filter, TalkableFilters)
		switch (filter->filterChat(chat))
		{
			case TalkableFilter::Accepted:
				return true;
			case TalkableFilter::Undecided:
				break;
			case TalkableFilter::Rejected:
				return false;
		}

	return true;
}

bool TalkableProxyModel::accept(const Buddy &buddy) const
{
	foreach (TalkableFilter *filter, TalkableFilters)
		switch (filter->filterBuddy(buddy))
		{
			case TalkableFilter::Accepted:
				return true;
			case TalkableFilter::Undecided:
				break;
			case TalkableFilter::Rejected:
				return false;
		}

	return true;
}

bool TalkableProxyModel::accept(const Contact &contact) const
{
	foreach (TalkableFilter *filter, TalkableFilters)
		switch (filter->filterContact(contact))
		{
			case TalkableFilter::Accepted:
				return true;
			case TalkableFilter::Undecided:
				break;
			case TalkableFilter::Rejected:
				return false;
		}

	return accept(contact.ownerBuddy());
}

bool TalkableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	// only filter first-level
	if (sourceParent.isValid())
		return true;

	const QModelIndex &sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
	switch (sourceIndex.data(ItemTypeRole).value<int>())
	{
		case ChatRole:
			return accept(sourceIndex.data(ChatRole).value<Chat>());
		case BuddyRole:
			return accept(sourceIndex.data(BuddyRole).value<Buddy>());
		case ContactRole:
			return accept(sourceIndex.data(ContactRole).value<Contact>());
		default:
			Q_ASSERT(false);
	}

	return true;
}

void TalkableProxyModel::addFilter(TalkableFilter *filter)
{
	if (TalkableFilters.contains(filter))
		return;

	TalkableFilters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
	connect(filter, SIGNAL(filterChanged()), this, SIGNAL(invalidated()));

	emit invalidated();
}

void TalkableProxyModel::removeFilter(TalkableFilter *filter)
{
	if (TalkableFilters.removeAll(filter) <= 0)
		return;

	invalidateFilter();
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
	disconnect(filter, SIGNAL(filterChanged()), this, SIGNAL(invalidate()));

	emit invalidated();
}
