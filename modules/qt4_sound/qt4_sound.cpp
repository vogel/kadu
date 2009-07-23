/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QSound>
#include <QtGui/QApplication>

#include "../sound/sound.h"

#include "debug.h"
#include "gui/windows/message-box.h"

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
		MessageBox::msg("QSound API is not available on this platform");
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

