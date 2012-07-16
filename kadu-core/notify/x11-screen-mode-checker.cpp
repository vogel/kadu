/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtGui/QX11Info>

#include "x11-screen-mode-checker.h"


X11ScreenModeChecker::X11ScreenModeChecker()
{
	x11display = XOpenDisplay(0);
}

X11ScreenModeChecker::~X11ScreenModeChecker()
{
	XCloseDisplay(x11display);
}

bool X11ScreenModeChecker::isFullscreenAppActive()
{
	return X11_checkFullScreen(x11display);
}

bool X11ScreenModeChecker::isScreensaverActive()
{
	// org.freedesktop.ScreenSaver
	{
		QDBusInterface dbus("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", QDBusConnection::sessionBus());
		if (dbus.isValid())
		{
			QDBusReply<bool> reply = dbus.call("GetActive");
			if (reply.isValid() && reply.value())
				return true;
		}
	}
	// org.kde.screensaver
	{
		QDBusInterface dbus("org.kde.screensaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", QDBusConnection::sessionBus());
		if (dbus.isValid())
		{
			QDBusReply<bool> reply = dbus.call("GetActive");
			if (reply.isValid() && reply.value())
				return true;
		}
	}
	// org.gnome.ScreenSaver
	{
		QDBusInterface dbus("org.gnome.ScreenSaver", "/", "org.gnome.ScreenSaver", QDBusConnection::sessionBus());
		if (dbus.isValid())
		{
			QDBusReply<bool> reply = dbus.call("GetActive");
			if (reply.isValid() && reply.value())
				return true;
		}
	}

	return false;
}

