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

#include "misc/change-notifier.h"
#include "protocols/services/roster/roster-entry-status.h"

#include "roster-entry.h"

RosterEntry::RosterEntry(QObject *parent) :
		QObject(parent), Status(RosterEntryUnkown), StatusChangeNotifier(new ChangeNotifier(this))
{
}

RosterEntry::~RosterEntry()
{
}

void RosterEntry::setStatus(RosterEntryStatus status)
{
	if (Status == status)
		return;

	Status = status;
	StatusChangeNotifier->notify();
}

RosterEntryStatus RosterEntry::status() const
{
	return Status;
}

ChangeNotifier * RosterEntry::statusChangeNotifier() const
{
	return StatusChangeNotifier;
}

bool RosterEntry::requiresSynchronization() const
{
	return RosterEntryDirty == Status;
}

void RosterEntry::markDirty(bool dirty)
{
	if (RosterEntryDetached == Status)
		return;

	setStatus(dirty ? RosterEntryDirty : RosterEntrySynchronized);
}
