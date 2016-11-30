/*
 * %kadu copyright begin%
 * Copyright 2011, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "misc/memory.h"
#include "plugin/plugin-injected-factory.h"
#include "themes.h"

#include "sound-theme-manager.h"

SoundThemeManager::SoundThemeManager(QObject *parent) :
		QObject{parent}
{
}

SoundThemeManager::~SoundThemeManager()
{
}

void SoundThemeManager::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void SoundThemeManager::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void SoundThemeManager::init()
{
	themes();
}

void SoundThemeManager::applyTheme(const QString &themeName)
{
	m_themes->setTheme(themeName);
	auto entries = m_themes->getEntries();
	auto i = entries.constBegin();

	while (i != entries.constEnd())
	{
		m_configuration->deprecatedApi()->writeEntry("Sounds", i.key() + "_sound", m_themes->themePath() + i.value());
		++i;
	}
}

Themes * SoundThemeManager::themes()
{
	if (!m_themes)
		loadThemes();
	return m_themes.get();
}

void SoundThemeManager::loadThemes()
{
	m_themes = m_pluginInjectedFactory->makeUnique<Themes>("sounds", "sound.conf");
	m_themes->setPaths(m_configuration->deprecatedApi()->readEntry("Sounds", "SoundPaths").split('&', QString::SkipEmptyParts));

	auto soundThemes = themes()->themes();
	auto soundTheme = m_configuration->deprecatedApi()->readEntry("Sounds", "SoundTheme");
	if (!soundThemes.isEmpty() && (soundTheme != "Custom") && !soundThemes.contains(soundTheme))
	{
		soundTheme = "default";
		m_configuration->deprecatedApi()->writeEntry("Sounds", "SoundTheme", "default");
	}

	if (soundTheme != "custom")
		applyTheme(soundTheme);
}

#include "moc_sound-theme-manager.cpp"
