/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
	: QObject(parent), Socket(-1), Started(false), ReadNotifier(0), WriteNotifier(0), TimeoutTimer(0)
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

	if (-1 == Socket)
		return;

	// Old code was using 0 instead of -1 for invalid socket. Check whether we didn't forget to fix something.
	Q_ASSERT(0 != Socket);

	ReadNotifier = new QSocketNotifier(Socket, QSocketNotifier::Read, this);
	connect(ReadNotifier, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	WriteNotifier = new QSocketNotifier(Socket, QSocketNotifier::Write, this);
	connect(WriteNotifier, SIGNAL(activated(int)), this, SLOT(dataSent()));

	Started = true;

	enable();

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

	if (TimeoutTimer)
	{
		TimeoutTimer->stop();
		TimeoutTimer->deleteLater();
		TimeoutTimer = 0;
	}

	kdebugf2();
}

void GaduSocketNotifiers::disable()
{
	kdebugf();

	if (!Started)
		return;

	ReadNotifier->setEnabled(false);
	WriteNotifier->setEnabled(false);
	if (TimeoutTimer)
		TimeoutTimer->stop();
}

void GaduSocketNotifiers::enable()
{
	kdebugf();

	if (!Started)
		return;

	ReadNotifier->setEnabled(checkRead());
	WriteNotifier->setEnabled(checkWrite());

	int tout = timeout();
	if (tout > 0)
	{
		if (!TimeoutTimer)
		{
			TimeoutTimer = new QTimer(this);
			TimeoutTimer->setSingleShot(true);
			connect(TimeoutTimer, SIGNAL(timeout()), this, SLOT(socketTimeout()));
		}
		TimeoutTimer->start(tout);
	}
	else if (TimeoutTimer)
	{
		TimeoutTimer->stop();
		TimeoutTimer->deleteLater();
		TimeoutTimer = 0;
	}
}

void GaduSocketNotifiers::watchFor(int socket)
{
	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "notifier: %p, old socket: %d, socket: %d\n", this, Socket, socket);

	if (Socket == socket)
		return;

	Socket = socket;
	createSocketNotifiers();
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

#include "moc_gadu-socket-notifiers.cpp"
