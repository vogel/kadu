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
#include "core/core.h"
#include "misc/paths-provider.h"

ScreenShotConfiguration::ScreenShotConfiguration(QObject *parent) :
		QObject{parent}
{
	createDefaultConfiguration();
	configurationUpdated();
}

ScreenShotConfiguration::~ScreenShotConfiguration()
{
}

void ScreenShotConfiguration::createDefaultConfiguration()
{
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "fileFormat", "PNG");
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "use_short_jpg", true);
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "quality", -1);
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "path", Core::instance()->pathsProvider()->profilePath() + QLatin1String("images/"));
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "filenamePrefix", "shot");
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "paste_clause", true);
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "dir_size_warns", true);
	Core::instance()->configuration()->deprecatedApi()->addVariable("ScreenShot", "dir_size_limit", 10000);
}

void ScreenShotConfiguration::configurationUpdated()
{
	FileFormat = Core::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "fileFormat", "PNG");
	UseShortJpgExtension = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "use_short_jpg", true);
	Quality = Core::instance()->configuration()->deprecatedApi()->readNumEntry("ScreenShot", "quality", -1);
	ImagePath = Core::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "path", Core::instance()->pathsProvider()->profilePath() + QLatin1String("images/"));
	FileNamePrefix = Core::instance()->configuration()->deprecatedApi()->readEntry("ScreenShot", "filenamePrefix", "shot");
	PasteImageClauseIntoChatWidget = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "paste_clause", true);
	WarnAboutDirectorySize = Core::instance()->configuration()->deprecatedApi()->readBoolEntry("ScreenShot", "dir_size_warns", true);
	DirectorySizeLimit = Core::instance()->configuration()->deprecatedApi()->readNumEntry("ScreenShot", "dir_size_limit", 10000);
}

QString ScreenShotConfiguration::screenshotFileNameExtension()
{
	bool useShortJpg = useShortJpgExtension();
	QString extension = fileFormat();
	if (useShortJpg && extension == "jpeg")
		return QLatin1String("jpg");

	return extension;
}

#include "moc_screen-shot-configuration.cpp"
