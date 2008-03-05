/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CHANGED_NOTIFICATION_H
#define STATUS_CHANGED_NOTIFICATION_H

#include <QTextDocument>

#include "protocol_notification.h"
#include "notify.h"

class StatusChangedNotification : public ProtocolNotification
{
	Q_OBJECT
public:

	static void registerEvents(Notify * manager);
	static void unregisterEvents(Notify * manager);

	StatusChangedNotification(const QString &toStatus, const UserListElements &userListElements, const QString &protocolName);
	virtual ~StatusChangedNotification() {};

};

#endif // STATUS_CHANGED_NOTIFICATION_H
