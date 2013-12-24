/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "notify/notification-manager.h"

#include "configuration/gui/sound-configuration-ui-handler.h"
#include "notify/sound-notifier.h"

#include "sound-actions.h"
#include "sound-manager.h"
#include "sound-theme-manager.h"

#include "sound-plugin.h"

SoundPlugin::~SoundPlugin()
{
}

bool SoundPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	SoundThemeManager::createInstance();
	SoundManager::createInstance();
	SoundNotifier::createInstance();
	SoundConfigurationUiHandler::registerConfigurationUi();
	NotificationManager::instance()->registerNotifier(SoundNotifier::instance());
	SoundActions::registerActions();

	return true;

}

void SoundPlugin::done()
{
	SoundActions::unregisterActions();
	NotificationManager::instance()->unregisterNotifier(SoundNotifier::instance());
	SoundConfigurationUiHandler::unregisterConfigurationUi();
	SoundNotifier::destroyInstance();
	SoundManager::destroyInstance();
	SoundThemeManager::destroyInstance();
}

Q_EXPORT_PLUGIN2(sound, SoundPlugin)

#include "moc_sound-plugin.cpp"
