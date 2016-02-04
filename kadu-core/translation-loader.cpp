/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "translation-loader.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "misc/paths-provider.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>

TranslationLoader::TranslationLoader(QObject *parent) :
		QObject{parent}
{
}

TranslationLoader::~TranslationLoader()
{
}

void TranslationLoader::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void TranslationLoader::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void TranslationLoader::init()
{	
	auto lang = m_configuration->deprecatedApi()->readEntry("General", "Language", QLocale::system().name().left(2));

	m_qt.load("qt_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));
	m_qtbase.load("qtbase_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));
	m_qtdeclarative.load("qtdeclarative_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));
	m_qtmultimedia.load("qtmultimedia_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));
	m_qtquick1.load("qtquick1_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));
	m_qtscript.load("qtscript_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));
	m_qtxmlpatterns.load("qtxmlpatterns_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));
	m_kadu.load("kadu_" + lang, m_pathsProvider->dataPath() + QStringLiteral("translations"));

	QCoreApplication::installTranslator(&m_qt);
	QCoreApplication::installTranslator(&m_qtbase);
	QCoreApplication::installTranslator(&m_qtdeclarative);
	QCoreApplication::installTranslator(&m_qtmultimedia);
	QCoreApplication::installTranslator(&m_qtquick1);
	QCoreApplication::installTranslator(&m_qtscript);
	QCoreApplication::installTranslator(&m_qtxmlpatterns);
	QCoreApplication::installTranslator(&m_kadu);
}

void TranslationLoader::done()
{
	QCoreApplication::removeTranslator(&m_qt);
	QCoreApplication::removeTranslator(&m_qtbase);
	QCoreApplication::removeTranslator(&m_qtdeclarative);
	QCoreApplication::removeTranslator(&m_qtmultimedia);
	QCoreApplication::removeTranslator(&m_qtquick1);
	QCoreApplication::removeTranslator(&m_qtscript);
	QCoreApplication::removeTranslator(&m_qtxmlpatterns);
	QCoreApplication::removeTranslator(&m_kadu);
}
