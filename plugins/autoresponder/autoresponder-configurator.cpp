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
#include "core/application.h"

#include "autoresponder-configuration.h"
#include "autoresponder.h"

#include "autoresponder-configurator.h"

void AutoresponderConfigurator::setAutoresponder(AutoResponder *autoresponder)
{
	ConfigurableAutoresponder = autoresponder;

	createDefaultConfiguration();
	configurationUpdated();
}

void AutoresponderConfigurator::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("Autoresponder", "Autotext", QCoreApplication::translate("AutoresponderConfigurator", "I am busy."));
	Application::instance()->configuration()->deprecatedApi()->addVariable("Autoresponder", "OnlyFirstTime", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Autoresponder", "RespondConf", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Autoresponder", "StatusAvailable", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Autoresponder", "StatusBusy", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Autoresponder", "StatusInvisible", false);
}

void AutoresponderConfigurator::configurationUpdated()
{
	if (!ConfigurableAutoresponder)
		return;

	AutoresponderConfiguration configuration;
	configuration.setAutoRespondText(Application::instance()->configuration()->deprecatedApi()->readEntry("Autoresponder", "Autotext"));
	configuration.setRespondConferences(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Autoresponder", "RespondConf"));
	configuration.setRespondOnlyFirst(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Autoresponder", "OnlyFirstTime"));
	configuration.setStatusAvailable(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Autoresponder", "StatusAvailable"));
	configuration.setStatusBusy(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Autoresponder", "StatusBusy"));
	configuration.setStatusInvisible(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Autoresponder", "StatusInvisible"));

	ConfigurableAutoresponder->setConfiguration(configuration);
}
