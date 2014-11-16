/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
		m_state{RosterEntryState::Unknown},
		m_detached{false},
		m_remotelyDeleted{false}
{
}

RosterEntry::~RosterEntry()
{
}

void RosterEntry::setState(RosterEntryState state)
{
	if (m_state == state)
		return;

	m_state = state;
	m_changeNotifier.notify();
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

void RosterEntry::setDetached(bool detached)
{
	if (m_detached == detached)
		return;

	m_detached = detached;
	m_changeNotifier.notify();
}

bool RosterEntry::detached() const
{
	return m_detached;
}

void RosterEntry::setRemotelyDeleted(bool remotelyDeleted)
{
	if (m_remotelyDeleted == remotelyDeleted)
		return;

	m_remotelyDeleted = remotelyDeleted;
	m_changeNotifier.notify();
}

bool RosterEntry::remotelyDeleted() const
{
	return m_remotelyDeleted;
}

ChangeNotifier & RosterEntry::changeNotifier()
{
	return m_changeNotifier;
}

bool RosterEntry::isSynchronizing() const
{
	return RosterEntryState::SynchronizingFromRemote == m_state || RosterEntryState::SynchronizingToRemote == m_state;
}

bool RosterEntry::requiresSynchronization() const
{
	return !m_detached && RosterEntryState::HasLocalChanges == m_state;
}

bool RosterEntry::canAcceptRemoteUpdate() const
{
	return !m_detached && RosterEntryState::HasLocalChanges != m_state && !isSynchronizing();
}

#include "moc_roster-entry.cpp"
