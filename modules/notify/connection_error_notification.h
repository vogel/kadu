/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONNECTION_ERROR_NOTIFICATION_H
#define CONNECTION_ERROR_NOTIFICATION_H

#include "notification.h"
#include "notify.h"

class ConnectionErrorNotification : public Notification
{
	Q_OBJECT

	QString ErrorMessage;

public:

	static void registerEvent(Notify * manager);
	static void unregisterEvent(Notify * manager);

	ConnectionErrorNotification(const QString &errorMessage);
	virtual ~ConnectionErrorNotification() {};

	QString errorMessage() const;

};

#endif // CONNECTION_ERROR_NOTIFICATION_H
