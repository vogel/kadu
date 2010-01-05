/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtDBus>

#include "message_box.h"
#include "debug.h"
#include "misc.h"

#include "kadudbus.h"
#include "kadurootobject.h"

extern "C" KADU_EXPORT int dbus_init(bool firstLoad)
{
	dbus = new KaduDBus();
	return 0;
}

extern "C" KADU_EXPORT void dbus_close()
{
	delete dbus;
	dbus = NULL;
}

KaduDBus::KaduDBus() : QObject(NULL, "dbus")
{
	KaduRootObject *root = new KaduRootObject(this);
	QDBusConnection connection = QDBusConnection::sessionBus();
	connection.registerObject("/", root, QDBusConnection::ExportAllContents);
	connection.registerService("im.kadu");
}

KaduDBus::~KaduDBus()
{
}

KaduDBus* dbus;
