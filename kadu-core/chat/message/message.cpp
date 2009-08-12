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
		QObject(), MyChat(chat), Sender(sender), MyStatus(Unknown)
{
}

Message::Message(const Message &copyMe)
{
	*this = copyMe;
}

void Message::operator = (const Message &copyMe)
{
	MyChat = copyMe.MyChat;
	Sender = copyMe.Sender;
	Content = copyMe.Content;
	ReceiveDate = copyMe.ReceiveDate;
	SendDate = copyMe.SendDate;
	MyStatus = copyMe.MyStatus;
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

Message & Message::setStatus(Status status)
{
	if (status != MyStatus)
	{
		MyStatus = status;
		emit statusChanged(MyStatus);
	}

	return *this;
}
