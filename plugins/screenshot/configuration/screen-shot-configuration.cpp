/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "core/application.h"
#include "misc/paths-provider.h"

#include "screen-shot-configuration.h"

ScreenShotConfiguration * ScreenShotConfiguration::Instance = 0;

ScreenShotConfiguration * ScreenShotConfiguration::instance()
{
	return Instance;
}

void ScreenShotConfiguration::createInstance()
{
	if (!Instance)
		Instance = new ScreenShotConfiguration();
}

void ScreenShotConfiguration::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

ScreenShotConfiguration::ScreenShotConfiguration()
{
	createDefaultConfiguration();
	configurationUpdated();
}

ScreenShotConfiguration::~ScreenShotConfiguration()
{
}

void ScreenShotConfiguration::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "fileFormat", "PNG");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "use_short_jpg", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "quality", -1);
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "path", Application::instance()->pathsProvider()->profilePath() + QLatin1String("images/"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "filenamePrefix", "shot");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "paste_clause", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "dir_size_warns", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "dir_size_limit", 10000);
}

void ScreenShotConfiguration::configurationUpdated()
{
	FileFormat = Application::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "fileFormat", "PNG");
	UseShortJpgExtension = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "use_short_jpg", true);
	Quality = Application::instance()->configuration()->deprecatedApi()->readNumEntry("ScreenShot", "quality", -1);
	ImagePath = Application::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "path", Application::instance()->pathsProvider()->profilePath() + QLatin1String("images/"));
	FileNamePrefix = Application::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "filenamePrefix", "shot");
	PasteImageClauseIntoChatWidget = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "paste_clause", true);
	WarnAboutDirectorySize = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "dir_size_warns", true);
	DirectorySizeLimit = Application::instance()->configuration()->deprecatedApi()->readNumEntry("ScreenShot", "dir_size_limit", 10000);
}

QString ScreenShotConfiguration::screenshotFileNameExtension()
{
	bool useShortJpg = useShortJpgExtension();
	QString extension = fileFormat();
	if (useShortJpg && extension == "jpeg")
		return QLatin1String("jpg");

	return extension;
}
