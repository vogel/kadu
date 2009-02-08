/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "kadu.h"

#include "connection-timeout-timer.h"

ConnectionTimeoutTimer *ConnectionTimeoutTimer::Instance = 0;

ConnectionTimeoutTimer::ConnectionTimeoutTimer(QObject *parent)
	: QTimer(parent)
{
	start(config_file.readUnsignedNumEntry("Network", "TimeoutInMs"), true);
}

bool ConnectionTimeoutTimer::connectTimeoutRoutine(const QObject *receiver, const char *member)
{
	return connect(Instance, SIGNAL(timeout()), receiver, member);
}

void ConnectionTimeoutTimer::on()
{
	if (!Instance)
		Instance = new ConnectionTimeoutTimer(kadu);
}

void ConnectionTimeoutTimer::off()
{
	if (Instance)
	{
		Instance->stop();
		Instance->deleteLater();
		Instance = 0;
	}
}
