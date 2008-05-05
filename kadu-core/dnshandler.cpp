/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include "dnshandler.h"

DNSHandler::DNSHandler(const QString &marker, const QHostAddress &address)
	: marker(marker)
{
	kdebugf();
//	kdebugm(KDEBUG_WARNING, ">>>>>>>>>>%s\n", marker.local8Bit().data());

	if (address == QHostAddress())
		kdebugmf(KDEBUG_WARNING, "NULL ip address!\n");

	QHostInfo::lookupHost(address.toString(), this, SLOT(resultsReady(QHostInfo)));
	++counter;

	kdebugmf(KDEBUG_FUNCTION_END, "counter = %d\n", counter);
}

DNSHandler::~DNSHandler()
{
	--counter;
}

void DNSHandler::resultsReady(QHostInfo hostInfo)
{
	kdebugf();
//	kdebugm(KDEBUG_WARNING, "resultsReady>>>>>>>>>>%s\n", marker.local8Bit().data());

	if (hostInfo.hostName().isEmpty())
		emit result(marker, QString::null);
	else
		emit result(marker, hostInfo.hostName());
		

	deleteLater();
	kdebugf2();
}

int DNSHandler::counter = 0;
