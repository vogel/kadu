/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "status/status-container-manager.h"

#include "kadu-window-action-data-source.h"

KaduWindowActionDataSource::KaduWindowActionDataSource() :
		ForwardActionDataSource(0)
{
}

KaduWindowActionDataSource::~KaduWindowActionDataSource()
{
}

void KaduWindowActionDataSource::setForwardActionDataSource(ActionDataSource *forwardActionDataSource)
{
	ForwardActionDataSource = forwardActionDataSource;
}

ContactSet KaduWindowActionDataSource::contacts()
{
	return ForwardActionDataSource
			? ForwardActionDataSource->contacts()
			: ContactSet();
}

BuddySet KaduWindowActionDataSource::buddies()
{
	return ForwardActionDataSource
			? ForwardActionDataSource->buddies()
			: BuddySet();
}

Chat KaduWindowActionDataSource::chat()
{
	return ForwardActionDataSource
			? ForwardActionDataSource->chat()
			: Chat::null;
}

StatusContainer * KaduWindowActionDataSource::statusContainer()
{
	return StatusContainerManager::instance();
}

bool KaduWindowActionDataSource::hasContactSelected()
{
	return ForwardActionDataSource
			? ForwardActionDataSource->hasContactSelected()
			: false;
}
