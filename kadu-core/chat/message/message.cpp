/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/message/message-data.h"

#include "message.h"

Message::Message(Chat *chat, Contact sender) :
		Data(new MessageData(chat, sender))
{
	if (Data.data())
		connect(Data.data(), SIGNAL(statusChanged(Message::Status)),
				this, SIGNAL(statusChanged(Message::Status)));
}

Message::Message(const Message& copy) :
		Data(copy.Data)
{
	if (Data.data())
		connect(Data.data(), SIGNAL(statusChanged(Message::Status)),
				this, SIGNAL(statusChanged(Message::Status)));
}

Message::~Message()
{
}

void Message::operator = (const Message &copy)
{
	if (Data.data())
		disconnect(Data.data(), SIGNAL(statusChanged(Message::Status)),
				this, SIGNAL(statusChanged(Message::Status)));
	Data = copy.Data;
	if (Data.data())
		connect(Data.data(), SIGNAL(statusChanged(Message::Status)),
				this, SIGNAL(statusChanged(Message::Status)));
}

Chat * Message::chat() const
{
	return Data.data()
			? Data->chat()
			: 0;
}

Message & Message::setChat(Chat *chat)
{
	if (Data.data())
		Data->setChat(chat);
	return *this;
}

Contact Message::sender() const
{
	return Data.data()
			? Data->sender()
			: Contact::null;
}

Message & Message::setSender(Contact sender)
{
	if (Data.data())
		Data->setSender(sender);
	return *this;
}

QString Message::content() const
{
	return Data.data()
			? Data->content()
			: QString::null;
}

Message & Message::setContent(const QString &content)
{
	if (Data.data())
		Data->setContent(content);
	return *this;
}

QDateTime Message::receiveDate() const
{
	return Data.data()
			? Data->receiveDate()
			: QDateTime();
}

Message & Message::setReceiveDate(QDateTime receiveDate)
{
	if (Data.data())
		Data->setReceiveDate(receiveDate);
	return *this;
}

QDateTime Message::sendDate() const
{
	return Data.data()
			? Data->sendDate()
			: QDateTime();
}

Message & Message::setSendDate(QDateTime sendDate)
{
	if (Data.data())
		Data->setSendDate(sendDate);
	return *this;
}

Message::Status Message::status() const
{
	return Data.data()
			? Data->status()
			: Unknown;
}

Message & Message::setStatus(Message::Status status)
{
	if (Data.data())
		Data->setStatus(status);
	return *this;
}
