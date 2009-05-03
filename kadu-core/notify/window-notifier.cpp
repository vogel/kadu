/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/windows/window-notifier-window.h"
#include "notify/notification-manager.h"

#include "config_file.h"
#include "debug.h"
#include "icons-manager.h"

#include "window-notifier.h"

/**
 * @ingroup window_notify
 * @{
 */

WindowNotifier::WindowNotifier(QObject *parent) :
		Notifier("Window", IconsManager::instance()->loadIcon("Information"), parent)
{
	kdebugf();

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
	window->raise();

	kdebugf2();
}

void WindowNotifier::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Window", true);
}

/** @} */

