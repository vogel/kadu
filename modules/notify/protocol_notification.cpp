/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "protocol_notification.h"

static QString getProtocolName(const QObject * const object)
{
	const ProtocolNotification * const notification = dynamic_cast<const ProtocolNotification * const>(object);
	if (notification)
		return notification->protocolName();
	else
		return "";
}

ProtocolNotification::ProtocolNotification(const QString &type, const QString &icon, const UserListElements &userListElements, const QString &protocolName)
	: Notification(type, icon, userListElements), ProtocolName(protocolName)
{
	KaduParser::registerObjectTag("protocol", getProtocolName);
}

ProtocolNotification::~ProtocolNotification()
{
	KaduParser::unregisterObjectTag("protocol", getProtocolName);
}

void ProtocolNotification::setProtocolName(const QString &name)
{
	ProtocolName = name;
}

QString ProtocolNotification::protocolName() const
{
	return ProtocolName;
}
