/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"

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
	config_file.addVariable("Autoresponder", "Autotext", QCoreApplication::translate("AutoresponderConfigurator", "I am busy."));
	config_file.addVariable("Autoresponder", "OnlyFirstTime", true);
	config_file.addVariable("Autoresponder", "RespondConf", true);
	config_file.addVariable("Autoresponder", "StatusAvailable", false);
	config_file.addVariable("Autoresponder", "StatusBusy", true);
	config_file.addVariable("Autoresponder", "StatusInvisible", false);
}

void AutoresponderConfigurator::configurationUpdated()
{
	if (!ConfigurableAutoresponder)
		return;

	AutoresponderConfiguration configuration;
	configuration.setAutoRespondText(config_file.readEntry("Autoresponder", "Autotext"));
	configuration.setRespondConferences(config_file.readBoolEntry("Autoresponder", "RespondConf"));
	configuration.setRespondOnlyFirst(config_file.readBoolEntry("Autoresponder", "OnlyFirstTime"));
	configuration.setStatusAvailable(config_file.readBoolEntry("Autoresponder", "StatusAvailable"));
	configuration.setStatusBusy(config_file.readBoolEntry("Autoresponder", "StatusBusy"));
	configuration.setStatusInvisible(config_file.readBoolEntry("Autoresponder", "StatusInvisible"));

	ConfigurableAutoresponder->setConfiguration(configuration);
}
