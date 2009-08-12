/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "message.h"

Message::Message(Chat *chat, Contact sender) :
		MyChat(chat), Sender(sender)
{
}

Message & Message::setChat(Chat *chat)
{
	MyChat = chat;
}

Message & Message::setSender(Contact sender)
{
	Sender = sender;
	return *this;
}

Message & Message::setContent(const QString &content)
{
	Content = content;
	return *this;
}

Message & Message::setReceiveDate(QDateTime receiveDate)
{
	ReceiveDate = receiveDate;
	return *this;
}

Message & Message::setSendDate(QDateTime sendDate)
{
	SendDate = sendDate;
	return *this;
}
