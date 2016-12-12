/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-module.h"

#include "windows/status-window-service.h"
#include "status/remote-status-request-handler.h"
#include "status/all-accounts-status-container.h"
#include "status/description-manager.h"
#include "status/status-configuration-holder.h"
#include "status/status-notification-service.h"
#include "status/status-setter.h"
#include "status/status-type-manager.h"

StatusModule::StatusModule()
{
	add_type<AllAccountsStatusContainer>();
	add_type<DescriptionManager>();
	add_type<RemoteStatusRequestHandler>();
	add_type<StatusConfigurationHolder>();
	add_type<StatusNotificationService>();
	add_type<StatusSetter>();
	add_type<StatusTypeManager>();
	add_type<StatusWindowService>();
}

StatusModule::~StatusModule()
{
}
