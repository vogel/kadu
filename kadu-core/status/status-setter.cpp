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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "status/status-changer-manager.h"
#include "status/status-container-manager.h"
#include "status/status-type-manager.h"

#include "status-setter.h"

StatusSetter * StatusSetter::Instance = 0;

StatusSetter * StatusSetter::instance()
{
	if (0 == Instance)
		Instance = new StatusSetter();

	return Instance;
}

StatusSetter::StatusSetter() :
		CoreInitialized(false)
{
	configurationUpdated();
}

StatusSetter::~StatusSetter()
{
}

void StatusSetter::setDefaultStatus(StatusContainer *statusContainer)
{
	Status status = statusContainer->loadStatus();

	if (!StartupLastDescription)
		status.setDescription(StartupDescription);

	if (StartupStatus != "LastStatus")
		status.setType(StatusTypeManager::instance()->fromName(StartupStatus));

	if (StatusTypeNone == status.type())
		status.setType(StatusTypeOnline);
	else if (StatusTypeOffline == status.type() && OfflineToInvisible)
		status.setType(StatusTypeInvisible);

	StatusSetter::instance()->setStatusManually(statusContainer, status);
}

void StatusSetter::coreInitialized()
{
	CoreInitialized = true;
	triggerAllStatusContainerRegistered();
}

void StatusSetter::configurationUpdated()
{
	StartupStatus = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "StartupStatus");
	StartupLastDescription = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "StartupLastDescription");
	StartupDescription = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "StartupDescription");
	OfflineToInvisible = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline") && StartupStatus != "Offline";

	if (StartupStatus.isEmpty())
		StartupStatus = "LastStatus";
	else if (StartupStatus == "Busy")
		StartupStatus =  "Away";
}

void StatusSetter::statusContainerRegistered(StatusContainer *statusContainer)
{
	if (CoreInitialized)
		setDefaultStatus(statusContainer);
}

void StatusSetter::statusContainerUnregistered(StatusContainer *statusContainer)
{
	Q_UNUSED(statusContainer);
}

void StatusSetter::setStatusManually(StatusContainer *statusContainer, Status status)
{
	StatusChangerManager::instance()->setStatusManually(statusContainer, status);
}

Status StatusSetter::manuallySetStatus(StatusContainer *statusContainer)
{
	return StatusChangerManager::instance()->manuallySetStatus(statusContainer);
}
