/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "storable-status-container.h"

#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "storage/storable-object.h"

StorableStatusContainer::StorableStatusContainer(StorableObject *storableObject, QObject *parent) :
		StatusContainer{parent},
		m_storableObject{storableObject}
{
}

StorableStatusContainer::~StorableStatusContainer()
{
}

void StorableStatusContainer::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void StorableStatusContainer::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

StatusTypeManager * StorableStatusContainer::statusTypeManager() const
{
	return m_statusTypeManager;
}

Status StorableStatusContainer::loadStatus()
{
	if (!m_storableObject->isValidStorage())
		return Status();

	QString name = m_storableObject->loadValue<QString>("LastStatusName");
	QString description = m_storableObject->loadValue<QString>("LastStatusDescription");

	// if no status is available in storage, then this status container is a new one
	// so we need to connect ASAP
	if (name.isEmpty())
		name = "Online";

	Status status;
	status.setType(m_statusTypeManager, m_statusTypeManager->fromName(name));
	status.setDescription(description);

	return status;
}

void StorableStatusContainer::storeStatus(Status status)
{
	if (!m_storableObject->isValidStorage())
		return;

	m_storableObject->storeValue("LastStatusDescription", status.description());
	m_storableObject->storeValue("LastStatusName", m_statusTypeManager->statusTypeData(status.type()).name());

	m_configurationManager->flush();
}

#include "moc_storable-status-container.cpp"
