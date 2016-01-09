/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-setter.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "status/status-changer-manager.h"
#include "status/status-container-manager.h"
#include "status/status-type-manager.h"

StatusSetter::StatusSetter(QObject *parent) :
		QObject{parent},
		CoreInitialized{false}
{
}

StatusSetter::~StatusSetter()
{
}

void StatusSetter::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void StatusSetter::setStatusChangerManager(StatusChangerManager *statusChangerManager)
{
	m_statusChangerManager = statusChangerManager;
}

void StatusSetter::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void StatusSetter::init()
{
	configurationUpdated();
}

void StatusSetter::setDefaultStatus(StatusContainer *statusContainer)
{
	Status status = statusContainer->loadStatus();

	if (!StartupLastDescription)
		status.setDescription(StartupDescription);

	if (StartupStatus != "LastStatus")
		status.setType(m_statusTypeManager->fromName(StartupStatus));

	if (StatusTypeNone == status.type())
		status.setType(StatusTypeOnline);
	else if (StatusTypeOffline == status.type() && OfflineToInvisible)
		status.setType(StatusTypeInvisible);

	setStatusManually(statusContainer, status);
}

void StatusSetter::coreInitialized()
{
	CoreInitialized = true;
	triggerAllStatusContainerRegistered();
}

void StatusSetter::configurationUpdated()
{
	StartupStatus = m_configuration->deprecatedApi()->readEntry("General", "StartupStatus");
	StartupLastDescription = m_configuration->deprecatedApi()->readBoolEntry("General", "StartupLastDescription");
	StartupDescription = m_configuration->deprecatedApi()->readEntry("General", "StartupDescription");
	OfflineToInvisible = m_configuration->deprecatedApi()->readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline") && StartupStatus != "Offline";

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
	m_statusChangerManager->setStatusManually(statusContainer, status);
}

Status StatusSetter::manuallySetStatus(StatusContainer *statusContainer)
{
	return m_statusChangerManager->manuallySetStatus(statusContainer);
}
