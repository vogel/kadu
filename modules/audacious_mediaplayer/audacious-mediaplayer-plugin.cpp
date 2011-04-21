/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@intera.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QtCore/QDateTime>

#include "../mediaplayer/mediaplayer.h"
#include "../mediaplayer/mpris_mediaplayer.h"

#include "debug.h"
#include "exports.h"

#include "audacious-mediaplayer-plugin.h"

AudaciousMediaplayerPlugin::~AudaciousMediaplayerPlugin()
{
}

int AudaciousMediaplayerPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	Audacious = new MPRISMediaPlayer("Audacious", "org.mpris.audacious");
	bool res = MediaPlayer::instance()->registerMediaPlayer(Audacious, Audacious);
	return res ? 0 : 1;
}

void AudaciousMediaplayerPlugin::done()
{
	MediaPlayer::instance()->unregisterMediaPlayer();
	delete Audacious;
	Audacious = 0;
}

Q_EXPORT_PLUGIN2(audacious_mediaplayer, AudaciousMediaplayerPlugin)
