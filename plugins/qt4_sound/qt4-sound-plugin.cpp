/*
 * %kadu copyright begin%
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/windows/message-dialog.h"

#include "plugins/sound/sound-manager.h"

#include "qt4_sound.h"

#include "qt4-sound-plugin.h"

Qt4SoundPlugin::~Qt4SoundPlugin()
{
}

int Qt4SoundPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (!QSound::isAvailable())
	{
		MessageDialog::show(KaduIcon("dialog-error"), QObject::tr("Kadu"), QObject::tr("QSound API is not available on this platform"));
		return 1;
	}

	QtSound4Player::createInstance();
	SoundManager::instance()->setPlayer(QtSound4Player::instance());

	return 0;
}

void Qt4SoundPlugin::done()
{
	SoundManager::instance()->setPlayer(0);
	QtSound4Player::destroyInstance();
}

Q_EXPORT_PLUGIN2(qt4_sound, Qt4SoundPlugin)
