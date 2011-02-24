/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>

#include "debug.h"

#include "gadu-socket-notifiers.h"

GaduSocketNotifiers::GaduSocketNotifiers(QObject *parent)
	: QObject(parent), Socket(0), Started(false), ReadNotifier(0), WriteNotifier(0), Lock(false)
{
	kdebugf();
	kdebugf2();
}

GaduSocketNotifiers::~GaduSocketNotifiers()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void GaduSocketNotifiers::createSocketNotifiers()
{
	kdebugf();

	deleteSocketNotifiers();

	if (0 >= Socket)
		return;

	ReadNotifier = new QSocketNotifier(Socket, QSocketNotifier::Read, this);
	connect(ReadNotifier, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	if (!checkRead())
		ReadNotifier->setEnabled(false);

	WriteNotifier = new QSocketNotifier(Socket, QSocketNotifier::Write, this);
	connect(WriteNotifier, SIGNAL(activated(int)), this, SLOT(dataSent()));
	if (!checkWrite())
		WriteNotifier->setEnabled(false);

	TimeoutTimer = new QTimer();
	TimeoutTimer->setSingleShot(true);
	connect(TimeoutTimer, SIGNAL(timeout()), this, SLOT(socketTimeout()));

	Started = true;

	int tout = timeout();
	if (0 < tout)
		TimeoutTimer->start(tout);

	kdebugf2();
}

void GaduSocketNotifiers::deleteSocketNotifiers()
{
	kdebugf();

	if (!Started)
		return;

	Started = false;

	ReadNotifier->setEnabled(false);
	ReadNotifier->deleteLater();
	ReadNotifier = 0;

	WriteNotifier->setEnabled(false);
	WriteNotifier->deleteLater();
	WriteNotifier = 0;

	TimeoutTimer->stop();
	TimeoutTimer->deleteLater();
	TimeoutTimer = 0;

	kdebugf2();
}

void GaduSocketNotifiers::disable()
{
	kdebugf();

	if (!Started)
		return;

	ReadNotifier->setEnabled(false);
	WriteNotifier->setEnabled(false);
	TimeoutTimer->stop();
}

void GaduSocketNotifiers::enable()
{
	kdebugf();

	if (!Started || Lock)
		return;

	if (checkRead())
		ReadNotifier->setEnabled(true);
	if (checkWrite())
		WriteNotifier->setEnabled(true);

	int tout = timeout();
	if (0 < tout)
		TimeoutTimer->start(tout);
}

void GaduSocketNotifiers::watchFor(int socket)
{
	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "notifier: %p, old socket: %d, socket: %d\n", this, Socket, socket);

	if (Socket == socket)
		return;

	Socket = socket;
	createSocketNotifiers();
}

void GaduSocketNotifiers::lock()
{
	kdebugf();

	Lock = true;
}

void GaduSocketNotifiers::unlock()
{
	kdebugf();

	Lock = false;
	enable();
}

void GaduSocketNotifiers::socketTimeout()
{
	kdebugf();

	if (!handleSoftTimeout())
		connectionTimeout();
}

void GaduSocketNotifiers::dataReceived()
{
	kdebugf();

	disable();
	socketEvent();
	enable();

	kdebugf2();
}

void GaduSocketNotifiers::dataSent()
{
	kdebugf();

	disable();
	socketEvent();
	enable();

	kdebugf2();
}
