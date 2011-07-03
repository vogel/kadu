/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "mediaplayer.h"

#include "mediaplayer-plugin.h"

MediaplayerPlugin::~MediaplayerPlugin()
{
}

int MediaplayerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MediaPlayer::createInstance();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/mediaplayer.ui"));
	MainConfigurationWindow::registerUiHandler(MediaPlayer::instance());

	return 0;
}

void MediaplayerPlugin::done()
{
	MainConfigurationWindow::unregisterUiHandler(MediaPlayer::instance());
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/mediaplayer.ui"));

	MediaPlayer::destroyInstance();
}

Q_EXPORT_PLUGIN2(mediaplayer, MediaplayerPlugin)
