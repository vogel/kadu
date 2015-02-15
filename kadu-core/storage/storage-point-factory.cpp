/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "storage-point-factory.h"

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "misc/memory.h"
#include "storage/storage-point.h"

StoragePointFactory::StoragePointFactory(QObject *parent) :
		QObject{parent},
		m_configurationFile{}
{
}

StoragePointFactory::~StoragePointFactory()
{
}

void StoragePointFactory::setConfigurationFile(ConfigurationApi *configurationFile)
{
	m_configurationFile = configurationFile;
}

std::unique_ptr<StoragePoint> StoragePointFactory::createStoragePoint(const QString &nodeName, StoragePoint *parent)
{
	if (!m_configurationFile || nodeName.isEmpty())
		return {};

	if (!parent)
		return make_unique<StoragePoint>(m_configurationFile, m_configurationFile->getNode(nodeName));

	Q_ASSERT(parent->storage() == m_configurationFile);

	auto node = m_configurationFile->getNode(parent->point(), nodeName);
	return make_unique<StoragePoint>(m_configurationFile, node);
}
