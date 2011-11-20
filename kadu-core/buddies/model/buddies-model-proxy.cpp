/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "buddies/filter/abstract-buddy-filter.h"
#include "contacts/contact.h"
#include "contacts/filter/abstract-contact-filter.h"
#include "message/message-manager.h"
#include "model/roles.h"
#include "status/status-type.h"
#include "status/status.h"

#include "buddies-model.h"

#include "buddies-model-proxy.h"

BuddiesModelProxy::BuddiesModelProxy(QObject *parent) :
		QSortFilterProxyModel(parent), SortByStatus(true)
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
}

BuddiesModelProxy::~BuddiesModelProxy()
{

}

int BuddiesModelProxy::compareNames(const QString &n1, const QString &n2) const
{
	return BrokenStringCompare
		? n1.toLower().localeAwareCompare(n2.toLower())
		: n1.localeAwareCompare(n2);
}

void BuddiesModelProxy::setSortByStatus(bool sortByStatus)
{
	if (SortByStatus == sortByStatus)
		return;

	SortByStatus = sortByStatus;
	invalidate();
}

bool BuddiesModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	const Buddy &leftBuddy = left.data(BuddyRole).value<Buddy>();
	const Buddy &rightBuddy = right.data(BuddyRole).value<Buddy>();

	if (leftBuddy.contacts().isEmpty() && !rightBuddy.contacts().isEmpty())
		return false;
	if (!leftBuddy.contacts().isEmpty() && rightBuddy.contacts().isEmpty())
		return true;

	if (leftBuddy.isBlocked() && !rightBuddy.isBlocked())
		return false;
	if (!leftBuddy.isBlocked() && rightBuddy.isBlocked())
		return true;

	const bool leftHasUnreadMessages = leftBuddy.unreadMessagesCount() > 0;
	const bool rightHasUnreadMessages = rightBuddy.unreadMessagesCount() > 0;
	if (!leftHasUnreadMessages && rightHasUnreadMessages)
		return false;
	if (leftHasUnreadMessages && !rightHasUnreadMessages)
		return true;

	if (SortByStatus)
	{
		const Contact &leftContact = BuddyPreferredManager::instance()->preferredContact(leftBuddy, false);
		const Contact &rightContact = BuddyPreferredManager::instance()->preferredContact(rightBuddy, false);

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

	const int displayCompare = compareNames(leftBuddy.display(), rightBuddy.display());
	return displayCompare < 0;
}

bool BuddiesModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if (sourceParent.isValid())
	{
		const Contact &contact = sourceModel()->index(sourceRow, 0, sourceParent).data(ContactRole).value<Contact>();
		foreach (AbstractContactFilter *filter, ContactFilters)
			if (!filter->acceptContact(contact))
				return false;
	}
	else
	{
		const Buddy &buddy = sourceModel()->index(sourceRow, 0, sourceParent).data(BuddyRole).value<Buddy>();
		foreach (AbstractBuddyFilter *filter, BuddyFilters)
			if (!filter->acceptBuddy(buddy))
				return false;
			else if (filter->ignoreNextFilters(buddy))
				return true;
	}

	return true;
}

void BuddiesModelProxy::addFilter(AbstractBuddyFilter *filter)
{
	if (BuddyFilters.contains(filter))
		return;

	BuddyFilters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void BuddiesModelProxy::removeFilter(AbstractBuddyFilter *filter)
{
	if (BuddyFilters.removeAll(filter) <= 0)
		return;

	invalidateFilter();
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void BuddiesModelProxy::addFilter(AbstractContactFilter *filter)
{
	if (ContactFilters.contains(filter))
		return;

	ContactFilters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void BuddiesModelProxy::removeFilter(AbstractContactFilter *filter)
{
	if (ContactFilters.removeAll(filter) <= 0)
		return;

	invalidateFilter();
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}
