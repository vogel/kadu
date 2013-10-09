/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "proxy-action-context.h"

ProxyActionContext::ProxyActionContext() :
		ForwardActionContext(0)
{
}

ProxyActionContext::~ProxyActionContext()
{
}

void ProxyActionContext::setForwardActionContext(ActionContext *forwardActionContext)
{
	if (ForwardActionContext)
		disconnect(ForwardActionContext, 0, this, 0);

	ForwardActionContext = forwardActionContext;

	if (ForwardActionContext)
		connect(ForwardActionContext, SIGNAL(changed()), this, SIGNAL(changed()));

	emit changed();
}

ContactSet ProxyActionContext::contacts()
{
	return ForwardActionContext
			? ForwardActionContext->contacts()
			: ContactSet();
}

BuddySet ProxyActionContext::buddies()
{
	return ForwardActionContext
			? ForwardActionContext->buddies()
			: BuddySet();
}

Chat ProxyActionContext::chat()
{
	return ForwardActionContext
			? ForwardActionContext->chat()
			: Chat::null;
}

StatusContainer * ProxyActionContext::statusContainer()
{
	return StatusContainerManager::instance();
}

RoleSet ProxyActionContext::roles()
{
	return ForwardActionContext
			? ForwardActionContext->roles()
			: RoleSet();
}

#include "moc_proxy-action-context.cpp"
