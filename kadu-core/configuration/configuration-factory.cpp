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

#include "configuration-factory.h"

#include "configuration/configuration.h"
#include "configuration/configuration-storage.h"
#include "misc/paths-provider.h"

ConfigurationFactory::ConfigurationFactory(QObject *parent) :
		QObject{parent},
		m_pathsProvider{nullptr}
{
}

ConfigurationFactory::~ConfigurationFactory()
{
}

void ConfigurationFactory::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

qobject_ptr<Configuration> ConfigurationFactory::createConfiguration()
{
	auto profilePath = m_pathsProvider->profilePath();
	auto configurationStorage = make_qobject<ConfigurationStorage>(profilePath);

	return make_qobject<Configuration>(std::move(configurationStorage));
}

#include "moc_configuration-factory.cpp"
