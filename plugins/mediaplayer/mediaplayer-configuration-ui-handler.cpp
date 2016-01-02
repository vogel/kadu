/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "mediaplayer-configuration-ui-handler.h"

#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"

#include <QtCore/QCoreApplication>

const char *MediaPlayerSyntaxText = QT_TRANSLATE_NOOP
(
	"@default",
	"Syntax: %t - song title,\n%a - album, %r - artist, %f - file name,\n"
	"%l - song length (MM:SS), %c - current song position (MM:SS),\n"
	"%p - percents of played song, %n - player name, %v - player version\n"
);

const char *MediaPlayerChatShortCutsText = QT_TRANSLATE_NOOP
(
	"@default",
	"With this option enabled you'll be able to control\n"
	"your MediaPlayer in chat window by keyboard shortcuts:\n"
	"Win+ Enter/Backspace/Left/Right/Up/Down."
);

MediaplayerConfigurationUiHandler::MediaplayerConfigurationUiHandler(QObject *parent) :
		QObject{parent}
{
}

MediaplayerConfigurationUiHandler::~MediaplayerConfigurationUiHandler()
{
}

void MediaplayerConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("mediaplayer/signature"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("mediaplayer/signatures"), SLOT(setEnabled(bool)));
	mainConfigurationWindow->widget()->widgetById("mediaplayer/syntax")->setToolTip(QCoreApplication::translate("@default", MediaPlayerSyntaxText));
	mainConfigurationWindow->widget()->widgetById("mediaplayer/chatShortcuts")->setToolTip(QCoreApplication::translate("@default", MediaPlayerChatShortCutsText));
}

void MediaplayerConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void MediaplayerConfigurationUiHandler::mainConfigurationWindowApplied()
{
}

#include "moc_mediaplayer-configuration-ui-handler.cpp"
