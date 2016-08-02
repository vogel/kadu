/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"

#include "autoresponder-configuration.h"
#include "autoresponder-message-filter.h"

#include "autoresponder-configurator.h"

AutoresponderConfigurator::AutoresponderConfigurator(QObject *parent) :
		QObject{parent}
{
}

AutoresponderConfigurator::~AutoresponderConfigurator()
{
}

void AutoresponderConfigurator::setAutoresponderMessageFilter(AutoresponderMessageFilter *autoresponderMessageFilter)
{
	m_autoresponderMessageFilter = autoresponderMessageFilter;
}

void AutoresponderConfigurator::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void AutoresponderConfigurator::init()
{
	createDefaultConfiguration();
	configurationUpdated();
}

void AutoresponderConfigurator::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Autoresponder", "Autotext", QCoreApplication::translate("AutoresponderConfigurator", "I am busy."));
	m_configuration->deprecatedApi()->addVariable("Autoresponder", "OnlyFirstTime", true);
	m_configuration->deprecatedApi()->addVariable("Autoresponder", "RespondConf", true);
	m_configuration->deprecatedApi()->addVariable("Autoresponder", "StatusAvailable", false);
	m_configuration->deprecatedApi()->addVariable("Autoresponder", "StatusBusy", true);
	m_configuration->deprecatedApi()->addVariable("Autoresponder", "StatusInvisible", false);
}

void AutoresponderConfigurator::configurationUpdated()
{
	AutoresponderConfiguration configuration;
	configuration.setAutoRespondText(m_configuration->deprecatedApi()->readEntry("Autoresponder", "Autotext"));
	configuration.setRespondConferences(m_configuration->deprecatedApi()->readBoolEntry("Autoresponder", "RespondConf"));
	configuration.setRespondOnlyFirst(m_configuration->deprecatedApi()->readBoolEntry("Autoresponder", "OnlyFirstTime"));
	configuration.setStatusAvailable(m_configuration->deprecatedApi()->readBoolEntry("Autoresponder", "StatusAvailable"));
	configuration.setStatusBusy(m_configuration->deprecatedApi()->readBoolEntry("Autoresponder", "StatusBusy"));
	configuration.setStatusInvisible(m_configuration->deprecatedApi()->readBoolEntry("Autoresponder", "StatusInvisible"));

	m_autoresponderMessageFilter->setConfiguration(configuration);
}

#include "moc_autoresponder-configurator.cpp"
