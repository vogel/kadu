/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/windows/window-notifier-window.h"
#include "notify/notification-manager.h"

#include "configuration/configuration-file.h"

#include "activate.h"
#include "debug.h"
#include "icons-manager.h"

#include "window-notifier.h"

/**
 * @ingroup window_notify
 * @{
 */

WindowNotifier::WindowNotifier(QObject *parent) :
		Notifier("Window", QT_TRANSLATE_NOOP("@default", "Show a window with notification"), "dialog-information", parent)
{
	kdebugf();

	import_0_6_5_configuration();
	createDefaultConfiguration();
	NotificationManager::instance()->registerNotifier(this);

	kdebugf2();
}

WindowNotifier::~WindowNotifier()
{
	kdebugf();
	NotificationManager::instance()->unregisterNotifier(this);
	kdebugf2();
}

void WindowNotifier::notify(Notification *notification)
{
	kdebugf();

	WindowNotifierWindow *window = new WindowNotifierWindow(notification);
	window->show();
	_activateWindow(window);

	kdebugf2();
}

void WindowNotifier::import_0_6_5_configuration()
{
    	config_file.addVariable("Notify", "StatusChanged/ToAway_Window", config_file.readEntry("Notify", "StatusChanged/ToBusy_Window"));
}

void WindowNotifier::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Window", true);
}

/** @} */

