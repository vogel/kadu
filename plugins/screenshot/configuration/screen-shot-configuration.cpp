/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "screen-shot-configuration.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "misc/paths-provider.h"

ScreenShotConfiguration::ScreenShotConfiguration(QObject *parent) : QObject{parent}
{
}

ScreenShotConfiguration::~ScreenShotConfiguration()
{
}

void ScreenShotConfiguration::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ScreenShotConfiguration::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void ScreenShotConfiguration::init()
{
    createDefaultConfiguration();
    configurationUpdated();
}

void ScreenShotConfiguration::createDefaultConfiguration()
{
    m_configuration->deprecatedApi()->addVariable("ScreenShot", "fileFormat", "PNG");
    m_configuration->deprecatedApi()->addVariable("ScreenShot", "use_short_jpg", true);
    m_configuration->deprecatedApi()->addVariable("ScreenShot", "quality", -1);
    m_configuration->deprecatedApi()->addVariable(
        "ScreenShot", "path", m_pathsProvider->profilePath() + QStringLiteral("images/"));
    m_configuration->deprecatedApi()->addVariable("ScreenShot", "filenamePrefix", "shot");
    m_configuration->deprecatedApi()->addVariable("ScreenShot", "paste_clause", true);
    m_configuration->deprecatedApi()->addVariable("ScreenShot", "dir_size_warns", true);
    m_configuration->deprecatedApi()->addVariable("ScreenShot", "dir_size_limit", 10000);
}

void ScreenShotConfiguration::configurationUpdated()
{
    FileFormat = m_configuration->deprecatedApi()->readEntry("ScreenShot", "fileFormat", "PNG");
    UseShortJpgExtension = m_configuration->deprecatedApi()->readBoolEntry("ScreenShot", "use_short_jpg", true);
    Quality = m_configuration->deprecatedApi()->readNumEntry("ScreenShot", "quality", -1);
    ImagePath = m_configuration->deprecatedApi()->readEntry(
        "ScreenShot", "path", m_pathsProvider->profilePath() + QStringLiteral("images/"));
    FileNamePrefix = m_configuration->deprecatedApi()->readEntry("ScreenShot", "filenamePrefix", "shot");
    PasteImageClauseIntoChatWidget =
        m_configuration->deprecatedApi()->readBoolEntry("ScreenShot", "paste_clause", true);
    WarnAboutDirectorySize = m_configuration->deprecatedApi()->readBoolEntry("ScreenShot", "dir_size_warns", true);
    DirectorySizeLimit = m_configuration->deprecatedApi()->readNumEntry("ScreenShot", "dir_size_limit", 10000);
}

QString ScreenShotConfiguration::screenshotFileNameExtension()
{
    bool useShortJpg = useShortJpgExtension();
    QString extension = fileFormat();
    if (useShortJpg && extension == "jpeg")
        return QStringLiteral("jpg");

    return extension;
}

#include "moc_screen-shot-configuration.cpp"
