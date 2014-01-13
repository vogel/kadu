/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/emoticon-configuration.h"
#include "expander/emoticon-expander-dom-visitor-provider.h"
#include "gui/insert-emoticon-action.h"
#include "theme/emoticon-theme-manager.h"
#include "theme/gadu-emoticon-theme-loader.h"

#include "emoticon-configurator.h"

EmoticonConfigurator::EmoticonConfigurator() :
		ThemeManager(new EmoticonThemeManager())
{
	createDefaultConfiguration();
}

void EmoticonConfigurator::setInsertAction(InsertEmoticonAction *insertAction)
{
	InsertAction = insertAction;
}

void EmoticonConfigurator::setEmoticonExpanderProvider(EmoticonExpanderDomVisitorProvider *emoticonExpanderProvider)
{
	EmoticonExpanderProvider = emoticonExpanderProvider;
}

void EmoticonConfigurator::configure()
{
	configurationUpdated();
}

void EmoticonConfigurator::createDefaultConfiguration()
{
	config_file.addVariable("Chat", "EmoticonsPaths", QString());
	config_file.addVariable("Chat", "EnableEmoticonAnimations", 1 != config_file.readNumEntry("Chat", "EmoticonsStyle", 2));
	config_file.addVariable("Chat", "EmoticonsTheme", ThemeManager->defaultTheme());
	config_file.addVariable("Chat", "EnableEmoticons", config_file.readEntry("Chat", "EmoticonsTheme") != "None");
}

void EmoticonConfigurator::configurationUpdated()
{
	if (!EmoticonExpanderProvider && !InsertAction)
		return;

	ThemeManager->loadThemes();

	Configuration.setEnabled(config_file.readBoolEntry("Chat", "EnableEmoticons", true));
	Configuration.setAnimate(config_file.readBoolEntry("Chat", "EnableEmoticonAnimations", true));

	if (Configuration.enabled())
	{
		if (LastLoadedThemeName != config_file.readEntry("Chat", "EmoticonsTheme"))
		{
			LastLoadedThemeName = config_file.readEntry("Chat", "EmoticonsTheme");
			ThemeManager->setCurrentTheme(LastLoadedThemeName);

			GaduEmoticonThemeLoader loader;
			LastTheme = loader.loadEmoticonTheme(ThemeManager->currentTheme().path());
		}

		Configuration.setEmoticonTheme(LastTheme);
	}
	else
		Configuration.setEmoticonTheme(EmoticonTheme());

	if (InsertAction)
		InsertAction->setConfiguration(Configuration);
	if (EmoticonExpanderProvider)
		EmoticonExpanderProvider->setConfiguration(Configuration);
}

#include "moc_emoticon-configurator.cpp"
