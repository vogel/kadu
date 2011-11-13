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

#ifndef PROXY_ACTION_CONTEXT_H
#define PROXY_ACTION_CONTEXT_H

#include "gui/actions/action-context.h"

class ProxyActionContext : public ActionContext
{
	Q_OBJECT

	ActionContext *ForwardActionContext;

public:
	ProxyActionContext();
	virtual ~ProxyActionContext();

	void setForwardActionContext(ActionContext *forwardActionContext);

	virtual ContactSet contacts();
	virtual BuddySet buddies();
	virtual Chat chat();
	virtual StatusContainer * statusContainer();
	virtual RoleSet roles();

};

#endif // PROXY_ACTION_CONTEXT_H
