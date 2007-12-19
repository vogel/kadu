/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "protocol.h"

Protocol::Protocol(const QString &proto, const QString &id, QObject *parent, const char *name) :
	QObject(parent, name), ConnectionTime(), ProtocolID(proto), id(id), CurrentStatus(0), NextStatus(0)
{
}

Protocol::~Protocol()
{
	delete CurrentStatus;
	delete NextStatus;
}

const QDateTime &Protocol::connectionTime() const
{
	return ConnectionTime;
}

QString Protocol::sendMessage(UserListElement user, const QString &mesg)
{
	UserListElements users(user);

	return sendMessage(users, mesg);
}
