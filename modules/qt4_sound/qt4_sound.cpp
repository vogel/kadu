/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include "gui/windows/message-dialog.h"
#include "modules/sound/sound-manager.h"
#include "debug.h"

#include "qt4_sound.h"

QtSound4Player *QtSound4Player::Instance = 0;

/**
 * @ingroup qt4_sound
 * @{
 */
extern "C" KADU_EXPORT int qt4_sound_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	if (!QSound::isAvailable ())
	{
		MessageDialog::show("dialog-error", QObject::tr("Kadu"), QObject::tr("QSound API is not available on this platform"));
		return 1;
	}

	QtSound4Player::createInstance();
	SoundManager::instance()->setPlayer(QtSound4Player::instance());

	kdebugf2();
	return 0;
}
extern "C" KADU_EXPORT void qt4_sound_close()
{
	kdebugf();

	SoundManager::instance()->setPlayer(0);
	QtSound4Player::destroyInstance();

	kdebugf2();
}

void QtSound4Player::createInstance()
{
	if (!Instance)
		Instance = new QtSound4Player();
}

void QtSound4Player::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

QtSound4Player *QtSound4Player::instance()
{
	return Instance;
}

QtSound4Player::QtSound4Player()
{
	kdebugf();
	kdebugf2();
}

QtSound4Player::~QtSound4Player()
{
	kdebugf();
	kdebugf2();
}

void QtSound4Player::playSound(const QString &path, bool volCntrl, double vol)
{
	Q_UNUSED(volCntrl)
	Q_UNUSED(vol)

	kdebugf();
	QSound::play(path);
	kdebugf2();
}

QtSound4Player *qt4_player;

/** @} */

