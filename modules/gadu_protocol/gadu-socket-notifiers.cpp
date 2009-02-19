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

GaduSocketNotifiers::GaduSocketNotifiers(int fd, QObject *parent)
	: QObject(parent), Fd(fd), Snr(0), Snw(0)
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

void GaduSocketNotifiers::start()
{
	kdebugf();
	createSocketNotifiers();
	kdebugf2();
}

void GaduSocketNotifiers::stop()
{
	kdebugf();
	deleteSocketNotifiers();
	kdebugf2();
}

void GaduSocketNotifiers::createSocketNotifiers()
{
	kdebugf();

	if (0 == Fd)
		return;
	// read_socket_notifier
	Snr = new QSocketNotifier(Fd, QSocketNotifier::Read, this);
	connect(Snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	//write_socket_notifier
	Snw = new QSocketNotifier(Fd, QSocketNotifier::Write, this);
	connect(Snw, SIGNAL(activated(int)), this, SLOT(dataSent()));

	kdebugf2();
}

void GaduSocketNotifiers::deleteSocketNotifiers()
{
	kdebugf();

	if (Snr)
	{
		Snr->setEnabled(false);
		Snr->deleteLater();
		Snr = 0;
	}

	if (Snw)
	{
		Snw->setEnabled(false);
		Snw->deleteLater();
		Snw = 0;
	}

	kdebugf2();
}

void GaduSocketNotifiers::recreateSocketNotifiers()
{
	kdebugf();

	deleteSocketNotifiers();
	createSocketNotifiers();

	kdebugf2();
}

void GaduSocketNotifiers::dataReceived()
{
	kdebugf();

	if (Snr)
		Snr->setEnabled(false);

	if (checkRead())
		socketEvent();

	if (Snr)
		Snr->setEnabled(true);

	kdebugf2();
}

void GaduSocketNotifiers::dataSent()
{
	kdebugf();

	if (Snw)
		Snw->setEnabled(false);

	if (checkWrite())
		socketEvent();

	kdebugf2();
}
