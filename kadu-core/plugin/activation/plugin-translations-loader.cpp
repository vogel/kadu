/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-translations-loader.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "misc/memory.h"
#include "misc/paths-provider.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLatin1String>
#include <QtCore/QTranslator>

PluginTranslationsLoader::PluginTranslationsLoader(const QString &pluginName) noexcept :
		m_translator{make_unique<QTranslator>()}
{
	auto const lang = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Language");

	if (m_translator->load(pluginName + '_' + lang, Application::instance()->pathsProvider()->dataPath() + QLatin1String{"plugins/translations"}))
		QCoreApplication::installTranslator(m_translator.get());
	else
		m_translator.reset();
}

PluginTranslationsLoader::~PluginTranslationsLoader() noexcept
{
	QCoreApplication::removeTranslator(m_translator.get());
}
