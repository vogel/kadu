/*
 * %kadu copyright begin%
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
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

#include "debug.h"

#include "dnshandler.h"

DNSHandler::DNSHandler(const QString &marker, const QHostAddress &address)
	: marker(marker)
{
	kdebugf();
//	kdebugm(KDEBUG_WARNING, ">>>>>>>>>>%s\n", qPrintable(marker));

	if (address.isNull())
	{
		kdebugmf(KDEBUG_WARNING, "NULL ip address!\n");
	}

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
//	kdebugm(KDEBUG_WARNING, "resultsReady>>>>>>>>>>%s\n", qPrintable(marker));

	if (hostInfo.hostName().isEmpty())
		emit result(marker, QString());
	else
		emit result(marker, hostInfo.hostName());


	deleteLater();
	kdebugf2();
}

int DNSHandler::counter = 0;
