/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTextDocument>

#include "chat-service.h"

bool ChatService::sendMessage(Contact user, const QString &messageContent)
{
	ContactList users;
	users.append(user);
	QTextDocument document(messageContent);
	Message message = Message::parse(&document);
	return sendMessage(users, message);
}

bool ChatService::sendMessage(ContactList users, const QString &messageContent)
{
	QTextDocument document(messageContent);
	Message message = Message::parse(&document);
	return sendMessage(users, message);
}

bool ChatService::sendMessage(Contact user, Message &message)
{
	ContactList users;
	users.append(user);
	return sendMessage(users, message);
}
