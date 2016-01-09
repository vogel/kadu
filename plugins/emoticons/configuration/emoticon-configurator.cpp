/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"

#include "configuration/emoticon-configuration.h"
#include "core/core.h"
#include "expander/emoticon-expander-dom-visitor-provider.h"
#include "gui/insert-emoticon-action.h"
#include "theme/emoticon-theme-manager.h"
#include "theme/gadu-emoticon-theme-loader.h"

#include "emoticon-configurator.h"

EmoticonConfigurator::EmoticonConfigurator(QObject *parent) :
		ConfigurationHolder{parent},
		ThemeManager(new EmoticonThemeManager())
{
	createDefaultConfiguration();
}

EmoticonConfigurator::~EmoticonConfigurator()
{
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
	Core::instance()->configuration()->deprecatedApi()->addVariable("Chat", "EmoticonsPaths", QString());
	Core::instance()->configuration()->deprecatedApi()->addVariable("Chat", "EnableEmoticonAnimations", 1 != Core::instance()->configuration()->deprecatedApi()->readNumEntry("Chat", "EmoticonsStyle", 2));
	Core::instance()->configuration()->deprecatedApi()->addVariable("Chat", "EmoticonsTheme", ThemeManager->defaultTheme());
	Core::instance()->configuration()->deprecatedApi()->addVariable("Chat", "EnableEmoticons", Core::instance()->configuration()->deprecatedApi()->readEntry("Chat", "EmoticonsTheme") != "None");
}

void EmoticonConfigurator::configurationUpdated()
{
	if (!EmoticonExpanderProvider && !InsertAction)
		return;

	ThemeManager->loadThemes();

	Configuration.setEnabled(Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "EnableEmoticons", true));
	Configuration.setAnimate(Core::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "EnableEmoticonAnimations", true));

	if (Configuration.enabled())
	{
		if (LastLoadedThemeName != Core::instance()->configuration()->deprecatedApi()->readEntry("Chat", "EmoticonsTheme"))
		{
			LastLoadedThemeName = Core::instance()->configuration()->deprecatedApi()->readEntry("Chat", "EmoticonsTheme");
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
