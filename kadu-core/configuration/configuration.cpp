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

#include "configuration.h"

#include "configuration/configuration-api.h"
#include "configuration/configuration-storage.h"
#include "configuration/configuration-unusable-exception.h"
#include "configuration/deprecated-configuration-api.h"
#include "misc/memory.h"

#include <QtCore/QDateTime>

Configuration::Configuration(QObject *parent) :
		QObject{parent},
		m_configurationStorage{nullptr}
{
}

Configuration::~Configuration()
{
}

void Configuration::setConfigurationStorage(ConfigurationStorage *configurationStorage)
{
	m_configurationStorage = configurationStorage;
}

ConfigurationApi * Configuration::api() const
{
	return m_configurationApi.get();
}

DeprecatedConfigurationApi * Configuration::deprecatedApi() const
{
	return m_deprecatedConfigurationApi.get();
}

void Configuration::read()
{
	m_configurationApi = make_unique<ConfigurationApi>(m_configurationStorage->readConfiguration());
	m_deprecatedConfigurationApi = make_unique<DeprecatedConfigurationApi>(m_configurationApi.get(), QLatin1String("kadu.conf"));

	if (!m_configurationStorage->isUsable())
		throw ConfigurationUnusableException(m_configurationStorage->profilePath());
}

void Configuration::write()
{
	m_configurationApi->touch();
	m_configurationStorage->writeConfiguration("kadu-0.12.conf.xml", m_configurationApi->configuration());
}

void Configuration::backup()
{
	auto backupName = QString("kadu-0.12.conf.xml.backup.%1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss"));
	m_configurationApi->touch();
	m_configurationStorage->writeConfiguration(backupName, m_configurationApi->configuration());
}

#include "moc_configuration.cpp"
