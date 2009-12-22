/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "message-shared.h"

MessageShared::MessageShared(Chat chat, Message::Type type, Contact sender) :
		QObject(), MyChat(chat), Sender(sender), MyStatus(Message::StatusUnknown), MyType(type), Id(0)
{
}

MessageShared::~MessageShared()
{
}

MessageShared & MessageShared::setChat(Chat chat)
{
	MyChat = chat;
	return *this;
}

MessageShared & MessageShared::setSender(Contact sender)
{
	Sender = sender;
	return *this;
}

MessageShared & MessageShared::setContent(const QString &content)
{
	Content = content;
	return *this;
}

MessageShared & MessageShared::setReceiveDate(QDateTime receiveDate)
{
	ReceiveDate = receiveDate;
	return *this;
}

MessageShared & MessageShared::setSendDate(QDateTime sendDate)
{
	SendDate = sendDate;
	return *this;
}

MessageShared & MessageShared::setStatus(Message::Status status)
{
	if (status != MyStatus)
	{
		MyStatus = status;
		emit statusChanged(MyStatus);
	}

	return *this;
}

MessageShared& MessageShared::setType(Message::Type type)
{
	MyType = type;
	return *this;
}

MessageShared& MessageShared::setId(int id)
{
	Id = id;
	return *this;
}
