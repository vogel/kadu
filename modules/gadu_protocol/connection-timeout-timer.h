/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONNECTION_TIMEOUT_TIMER_H
#define CONNECTION_TIMEOUT_TIMER_H

#include <QtCore/QTimer>

class ConnectionTimeoutTimer : public QTimer
{
	ConnectionTimeoutTimer(QObject *parent = 0);
	static ConnectionTimeoutTimer *Instance;

public:
	static void on();
	static void off();
	static bool connectTimeoutRoutine(const QObject *receiver, const char *member);

};

#endif // CONNECTION_TIMEOUT_TIMER_H
