/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QSound>
#include <QtGui/QApplication>

#include "../sound/sound.h"

#include "debug.h"
#include "gui/windows/message-dialog.h"

#include "qt4_sound.h"

/**
 * @ingroup qt4_sound
 * @{
 */
extern "C" KADU_EXPORT int qt4_sound_init(bool firstLoad)
{
	kdebugf();

	if (!QSound::isAvailable ())
	{
		MessageDialog::msg("QSound API is not available on this platform");
		return 1;
	}

	qt4_player = new Qt4Player();

	kdebugf2();
	return 0;
}
extern "C" KADU_EXPORT void qt4_sound_close()
{
	kdebugf();

	delete qt4_player;
	qt4_player = 0;

	kdebugf2();
}

Qt4Player::Qt4Player()
{
	kdebugf();

	connect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

Qt4Player::~Qt4Player()
{
	kdebugf();

	disconnect(sound_manager, SIGNAL(playSound(const QString &, bool, double)),
			this, SLOT(playSound(const QString &, bool, double)));

	kdebugf2();
}

void Qt4Player::playSound(const QString &s, bool volCntrl, double vol)
{
	kdebugf();

	QSound::play(s);

	kdebugf2();
}

Qt4Player *qt4_player;

/** @} */

