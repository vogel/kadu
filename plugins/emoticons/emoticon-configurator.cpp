/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"

#include "emoticon-configuration.h"
#include "emoticons-manager.h"
#include "emoticon-theme-manager.h"

#include "emoticon-configurator.h"

EmoticonConfigurator::EmoticonConfigurator(EmoticonThemeManager *themeManager) :
		ThemeManager(themeManager)
{
	Q_ASSERT(ThemeManager);
}

void EmoticonConfigurator::setEmoticonsManager(EmoticonsManager *emoticonsManager)
{
	Manager = emoticonsManager;

	configurationUpdated();
}

void EmoticonConfigurator::createDefaultConfiguration()
{
	config_file.addVariable("Chat", "EmoticonsPaths", QString());
	config_file.addVariable("Chat", "EmoticonsStyle", 2);
	config_file.addVariable("Chat", "EmoticonsTheme", ThemeManager->defaultTheme());
}

void EmoticonConfigurator::configurationUpdated()
{
	if (!Manager)
		return;

	EmoticonConfiguration configuration;
	ThemeManager->setCurrentTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	configuration.setAnimated(1 != config_file.readNumEntry("Chat", "EmoticonsStyle", 2));
	configuration.setEmoticonTheme(ThemeManager->currentTheme());

	Manager.data()->setConfiguration(configuration);
}
