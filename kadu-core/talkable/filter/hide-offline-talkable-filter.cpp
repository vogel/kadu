/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hide-offline-talkable-filter.h"

#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"

HideOfflineTalkableFilter::HideOfflineTalkableFilter(QObject *parent) :
		TalkableFilter{parent},
		m_enabled{false}
{
}

HideOfflineTalkableFilter::~HideOfflineTalkableFilter()
{
}

void HideOfflineTalkableFilter::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

void HideOfflineTalkableFilter::setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager)
{
	m_buddyPreferredManager = buddyPreferredManager;
}

void HideOfflineTalkableFilter::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

TalkableFilter::FilterResult HideOfflineTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (!m_enabled)
		return Undecided;

	auto contact = m_buddyPreferredManager->preferredContact(buddy, false);
	if (!contact)
		return Rejected;

	return filterContact(contact);
}

TalkableFilter::FilterResult HideOfflineTalkableFilter::filterContact(const Contact &contact)
{
	if (!m_enabled)
		return Undecided;

	auto status = contact.currentStatus();
	if (status.isDisconnected())
		return Rejected;
	else
		return Undecided;
}

void HideOfflineTalkableFilter::setEnabled(bool enabled)
{
	if (m_enabled == enabled)
		return;

	m_enabled = enabled;
	emit filterChanged();

	// Without it, Kadu crashes either on login or on clicking on the contacts list
	// because of QTBUG-27122 in Qt 4.8.3. Though, I'm not 100% sure if we don't need
	// it anyways to be correct...
	// TODO Qt5: Check whether we actually need it (QTBUG-27122 is fixed in Qt5) and remove if not.
	if (enabled)
	{
		connect(m_buddyManager, SIGNAL(buddyUpdated(Buddy)), this, SIGNAL(filterChanged()));
		connect(m_contactManager, SIGNAL(contactUpdated(Contact)), this, SIGNAL(filterChanged()));
	}
	else
	{
		disconnect(m_buddyManager, SIGNAL(buddyUpdated(Buddy)), this, SIGNAL(filterChanged()));
		disconnect(m_contactManager, SIGNAL(contactUpdated(Contact)), this, SIGNAL(filterChanged()));
	}
}

#include "moc_hide-offline-talkable-filter.cpp"
