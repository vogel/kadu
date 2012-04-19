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
#include "protocols/services/roster/roster-entry-state.h"

#include "roster-entry.h"

RosterEntry::RosterEntry(QObject *parent) :
		QObject(parent), State(RosterEntryUnkown), StateChangeNotifier(new ChangeNotifier(this))
{
}

RosterEntry::~RosterEntry()
{
}

void RosterEntry::setState(RosterEntryState state)
{
	if (State == state)
		return;

	State = state;
	StateChangeNotifier->notify();
}

RosterEntryState RosterEntry::state() const
{
	return State;
}

ChangeNotifier * RosterEntry::stateChangeNotifier() const
{
	return StateChangeNotifier;
}

bool RosterEntry::requiresSynchronization() const
{
	return RosterEntryDirty == State;
}

void RosterEntry::markDirty(bool dirty)
{
	if (RosterEntryDetached == State)
		return;

	setState(dirty ? RosterEntryDirty : RosterEntrySynchronized);
}
