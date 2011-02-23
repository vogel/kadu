/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "debug.h"

#include "configuration/gui/sound-configuration-ui-handler.h"
#include "notify/sound-notifier.h"

#include "sound-actions.h"
#include "sound-manager.h"
#include "sound-theme-manager.h"

extern "C" KADU_EXPORT int sound_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	SoundThemeManager::createInstance();
	SoundNotifier::createInstance();
	SoundConfigurationUiHandler::registerConfigurationUi();
	NotificationManager::instance()->registerNotifier(SoundNotifier::instance());
	SoundManager::createInstance();
	SoundActions::registerActions();

	return 0;
}

extern "C" KADU_EXPORT void sound_close()
{
	kdebugf();

	NotificationManager::instance()->unregisterNotifier(SoundNotifier::instance());
	SoundConfigurationUiHandler::unregisterConfigurationUi();
	SoundActions::unregisterActions();
	SoundManager::destroyInstance();
	SoundNotifier::destroyInstance();
	SoundThemeManager::destroyInstance();
}
