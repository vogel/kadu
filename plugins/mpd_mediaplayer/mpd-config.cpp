/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/kadu-paths.h"

#include "mpd-config.h"

MPDConfig::MPDConfig()
{
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/mpd_config.ui"));
	createDefaultConfiguration();
	Host = config_file.readEntry("MediaPlayer", "MPDHost");
	Port = config_file.readEntry("MediaPlayer", "MPDPort");
	Timeout = config_file.readEntry("MediaPlayer", "MPDTimeout");
}

MPDConfig::~MPDConfig()
{
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/mpd_config.ui"));
}

void MPDConfig::createDefaultConfiguration()
{
	config_file.addVariable("MediaPlayer", "MPDHost", "localhost");
	config_file.addVariable("MediaPlayer", "MPDPort", "6600");
	config_file.addVariable("MediaPlayer", "MPDTimeout", "10");
}

void MPDConfig::configurationUpdated()
{
	Host = config_file.readEntry("MediaPlayer", "MPDHost");
	Port = config_file.readEntry("MediaPlayer", "MPDPort");
	Timeout = config_file.readEntry("MediaPlayer", "MPDTimeout");
}
