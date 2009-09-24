/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_DATA_H
#define MESSAGE_DATA_H

#include <QtCore/QSharedData>

#include "chat/message/message.h"
#include "contacts/contact.h"

class Chat;

class MessageData : public QObject, public QSharedData
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageData)

	Chat *MyChat;
	Contact Sender;
	QString Content;
	QDateTime ReceiveDate;
	QDateTime SendDate;
	Message::Status MyStatus;
	Message::Type MyType;
	int Id;

public:
	MessageData(Chat *chat = 0, Message::Type type = Message::TypeUnknown, Contact sender = Contact::null);
	virtual ~MessageData();

	Chat * chat() const { return MyChat; }
	MessageData & setChat(Chat *chat);

	Contact sender() const { return Sender; }
	MessageData & setSender(Contact sender);

	QString content() const { return Content; }
	MessageData & setContent(const QString &content);

	QDateTime receiveDate() const { return ReceiveDate; }
	MessageData & setReceiveDate(QDateTime receiveDate);

	QDateTime sendDate() const { return SendDate; }
	MessageData & setSendDate(QDateTime sendDate);

	Message::Status status() const { return MyStatus; }
	MessageData & setStatus(Message::Status status);

	Message::Type type() const { return MyType; }
	MessageData & setType(Message::Type type);

	int id() const { return Id; }
	MessageData & setId(int id);

signals:
	void statusChanged(Message::Status);

};

#endif // MESSAGE_DATA_H
