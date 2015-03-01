/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "roster/roster-entry-state.h"

#include "roster-entry.h"

RosterEntry::RosterEntry(QObject *parent) :
		QObject{parent},
		m_state{RosterEntryState::Unknown}
{
}

RosterEntry::~RosterEntry()
{
}

void RosterEntry::setSynchronized()
{
	setState(RosterEntryState::Synchronized);
}

bool RosterEntry::setHasLocalChanges()
{
	if (m_state == RosterEntryState::SynchronizingToRemote)
		return false;
	if (m_state == RosterEntryState::SynchronizingFromRemote)
		return false;
	if (m_state == RosterEntryState::Detached)
		return false;

	setState(RosterEntryState::HasLocalChanges);
	return true;
}

void RosterEntry::setSynchronizingToRemote()
{
	setState(RosterEntryState::SynchronizingToRemote);
}

void RosterEntry::setSynchronizingFromRemote()
{
	setState(RosterEntryState::SynchronizingFromRemote);
}

void RosterEntry::setDetached()
{
	setState(RosterEntryState::Detached);
}

void RosterEntry::setState(RosterEntryState state)
{
	if (m_state == state)
		return;

	m_state = state;
	if (m_state == RosterEntryState::HasLocalChanges)
		m_hasLocalChangesNotifier.notify();
}

void RosterEntry::fixupInitialState()
{
	if (m_state == RosterEntryState::SynchronizingToRemote)
		m_state = RosterEntryState::HasLocalChanges;
}

RosterEntryState RosterEntry::state() const
{
	return m_state;
}

ChangeNotifier & RosterEntry::hasLocalChangesNotifier()
{
	return m_hasLocalChangesNotifier;
}

bool RosterEntry::requiresSynchronization() const
{
	if (m_state == RosterEntryState::HasLocalChanges)
		return true;
	return false;
}

#include "moc_roster-entry.cpp"
