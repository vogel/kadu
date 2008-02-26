/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dnshandler.h"
#include "debug.h"

DNSHandler::DNSHandler(const QString &marker, const QHostAddress &address) : DNSResolver(), marker(marker)
{
	kdebugf();
//	kdebugm(KDEBUG_WARNING, ">>>>>>>>>>%s\n", marker.local8Bit().data());

	if (address == QHostAddress())
		kdebugmf(KDEBUG_WARNING, "NULL ip address!\n");

	connect(&DNSResolver, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
	DNSResolver.setRecordType(Q3Dns::Ptr);
	DNSResolver.setLabel(address);
	++counter;

	kdebugmf(KDEBUG_FUNCTION_END, "counter = %d\n", counter);
}

DNSHandler::~DNSHandler()
{
	--counter;
}

void DNSHandler::resultsReady()
{
	kdebugf();
//	kdebugm(KDEBUG_WARNING, "resultsReady>>>>>>>>>>%s\n", marker.local8Bit().data());

	if (!DNSResolver.hostNames().isEmpty())
		emit result(marker, DNSResolver.hostNames()[0]);
	else
		emit result(marker, QString::null);

	deleteLater();
	kdebugf2();
}

int DNSHandler::counter = 0;
