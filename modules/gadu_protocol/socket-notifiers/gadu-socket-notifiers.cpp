/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSocketNotifier>

#include "debug.h"

#include "gadu-socket-notifiers.h"

GaduSocketNotifiers::GaduSocketNotifiers(QObject *parent)
	: QObject(parent), Socket(0), ReadNotifier(0), WriteNotifier(0)
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

	// read_socket_notifier
	ReadNotifier = new QSocketNotifier(Socket, QSocketNotifier::Read, this);
	connect(ReadNotifier, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	//write_socket_notifier
	WriteNotifier = new QSocketNotifier(Socket, QSocketNotifier::Write, this);
	connect(WriteNotifier, SIGNAL(activated(int)), this, SLOT(dataSent()));

	kdebugf2();
}

void GaduSocketNotifiers::deleteSocketNotifiers()
{
	kdebugf();

	if (ReadNotifier)
	{
		ReadNotifier->setEnabled(false);
		ReadNotifier->deleteLater();
		ReadNotifier = 0;
	}

	if (WriteNotifier)
	{
		WriteNotifier->setEnabled(false);
		WriteNotifier->deleteLater();
		WriteNotifier = 0;
	}

	kdebugf2();
}

void GaduSocketNotifiers::watchFor(int socket)
{
	if (Socket == socket)
		return;

	Socket = socket;
	createSocketNotifiers();
}

void GaduSocketNotifiers::watchWriting()
{
	if (WriteNotifier && checkWrite())
		WriteNotifier->setEnabled(true);
}

void GaduSocketNotifiers::dataReceived()
{
	kdebugf();

	if (ReadNotifier)
		ReadNotifier->setEnabled(false);

	if (checkRead())
		socketEvent();

	if (ReadNotifier)
		ReadNotifier->setEnabled(true);

	kdebugf2();
}

void GaduSocketNotifiers::dataSent()
{
	kdebugf();

	if (WriteNotifier)
		WriteNotifier->setEnabled(false);

	if (checkWrite())
		socketEvent();

	kdebugf2();
}
