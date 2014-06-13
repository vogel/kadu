/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration-storage-factory.h"

#include "configuration/configuration-storage.h"
#include "misc/paths-provider.h"

ConfigurationStorageFactory::ConfigurationStorageFactory(QObject *parent) :
		QObject{parent},
		m_pathsProvider{nullptr}
{
}

ConfigurationStorageFactory::~ConfigurationStorageFactory()
{
}

void ConfigurationStorageFactory::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

qobject_ptr<ConfigurationStorage> ConfigurationStorageFactory::createConfigurationStorage()
{
	return make_qobject<ConfigurationStorage>(m_pathsProvider->profilePath());
}

#include "moc_configuration-storage-factory.cpp"
