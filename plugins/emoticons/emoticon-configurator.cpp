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
#include "emoticon-expander-dom-visitor-provider.h"
#include "emoticon-theme-manager.h"
#include "gadu-emoticon-theme-loader.h"
#include "insert-emoticon-action.h"

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
	config_file.addVariable("Chat", "EmoticonsStyle", 2);
	config_file.addVariable("Chat", "EmoticonsTheme", ThemeManager->defaultTheme());
}

void EmoticonConfigurator::configurationUpdated()
{
	if (!EmoticonExpanderProvider && !InsertAction)
		return;

	ThemeManager->loadThemes(config_file.readEntry("Chat", "EmoticonsPaths").split('&', QString::SkipEmptyParts));

	if (LastLoadedThemeName == config_file.readEntry("Chat", "EmoticonsTheme"))
		return;

	LastLoadedThemeName = config_file.readEntry("Chat", "EmoticonsTheme");
	ThemeManager->setCurrentTheme(LastLoadedThemeName);

	GaduEmoticonThemeLoader loader;
	EmoticonTheme theme = loader.loadEmoticonTheme(ThemeManager->currentTheme().path());

	Configuration.setAnimated(1 != config_file.readNumEntry("Chat", "EmoticonsStyle", 2));
	Configuration.setEmoticonTheme(theme);

	if (InsertAction)
		InsertAction.data()->setConfiguration(Configuration);
	if (EmoticonExpanderProvider)
		EmoticonExpanderProvider.data()->setConfiguration(Configuration);
}
