/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy.h"

#include "non-account-buddy-filter.h"

NonAccountBuddyFilter::NonAccountBuddyFilter(QObject *parent) :
		AbstractBuddyFilter(parent)
{
}

NonAccountBuddyFilter::~NonAccountBuddyFilter()
{
}

void NonAccountBuddyFilter::setAccount(Account account)
{
	if (CurrentAccount != account)
	{
		CurrentAccount = account;
		emit filterChanged();
	}
}

bool NonAccountBuddyFilter::acceptBuddy(const Buddy &buddy)
{
	return CurrentAccount.isNull() || !buddy.hasContact(CurrentAccount);
}
