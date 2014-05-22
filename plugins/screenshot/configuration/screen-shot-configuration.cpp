/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "misc/kadu-paths.h"
#include "kadu-application.h"

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
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "fileFormat", "PNG");
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "use_short_jpg", true);
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "quality", -1);
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "path", KaduPaths::instance()->profilePath() + QLatin1String("images/"));
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "filenamePrefix", "shot");
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "paste_clause", true);
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "dir_size_warns", true);
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "dir_size_limit", 10000);
}

void ScreenShotConfiguration::configurationUpdated()
{
	FileFormat = KaduApplication::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "fileFormat", "PNG");
	UseShortJpgExtension = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "use_short_jpg", true);
	Quality = KaduApplication::instance()->configuration()->deprecatedApi()->readNumEntry("ScreenShot", "quality", -1);
	ImagePath = KaduApplication::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "path", KaduPaths::instance()->profilePath() + QLatin1String("images/"));
	FileNamePrefix = KaduApplication::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "filenamePrefix", "shot");
	PasteImageClauseIntoChatWidget = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "paste_clause", true);
	WarnAboutDirectorySize = KaduApplication::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "dir_size_warns", true);
	DirectorySizeLimit = KaduApplication::instance()->configuration()->deprecatedApi()->readNumEntry("ScreenShot", "dir_size_limit", 10000);
}

QString ScreenShotConfiguration::screenshotFileNameExtension()
{
	bool useShortJpg = useShortJpgExtension();
	QString extension = fileFormat();
	if (useShortJpg && extension == "jpeg")
		return QLatin1String("jpg");

	return extension;
}
