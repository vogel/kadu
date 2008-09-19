/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTextDocument>

#include "message.h"

#include "protocol.h"

Protocol::Protocol(const QString &proto, const QString &id, QObject *parent)
	: QObject(parent), ConnectionTime(), ProtocolID(proto), id(id), CurrentStatus(0), NextStatus(0)
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

bool Protocol::sendMessage(UserListElement user, const QString &messageContent)
{
	UserListElements users(user);
	QTextDocument document(messageContent);
	Message message = Message::parse(&document);
	return sendMessage(users, message);
}

bool Protocol::sendMessage(UserListElements users, const QString &messageContent)
{
	QTextDocument document(messageContent);
	Message message = Message::parse(&document);
	return sendMessage(users, message);
}

bool Protocol::sendMessage(UserListElement user, Message &message)
{
	UserListElements users(user);
	return sendMessage(users, message);
}
