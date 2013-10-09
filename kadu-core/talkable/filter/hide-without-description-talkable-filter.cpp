/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"

#include "hide-without-description-talkable-filter.h"

HideWithoutDescriptionTalkableFilter::HideWithoutDescriptionTalkableFilter(QObject *parent) :
		TalkableFilter(parent), Enabled(false)
{
}

HideWithoutDescriptionTalkableFilter::~HideWithoutDescriptionTalkableFilter()
{
}

TalkableFilter::FilterResult HideWithoutDescriptionTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (!Enabled)
		return Undecided;

	const Contact &contact = BuddyPreferredManager::instance()->preferredContact(buddy);
	if (contact.isNull())
		return Rejected;

	if (contact.currentStatus().description().isEmpty())
		return Rejected;
	else
		return Undecided;
}

void HideWithoutDescriptionTalkableFilter::setEnabled(bool enabled)
{
	if (Enabled == enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

#include "moc_hide-without-description-talkable-filter.cpp"
