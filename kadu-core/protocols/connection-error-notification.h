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

#include "notify/account-notification.h"

class NotifyEvent;

class ConnectionErrorNotification : public AccountNotification
{
	Q_OBJECT

	static NotifyEvent *ConnectionErrorNotifyEvent;
	static QMap<Account *, QStringList> ActiveErrors;
	QString ErrorServer;
	QString ErrorMessage;

public:
	static void registerEvent();
	static void unregisterEvent();

	static bool activeError(Account *account, const QString &errorMessage);

	ConnectionErrorNotification(Account *account, const QString &errorServer, const QString &errorMessage);
	virtual ~ConnectionErrorNotification();

	QString errorMessage() const;
	QString errorServer() const;

};

#endif // CONNECTION_ERROR_NOTIFICATION_H
