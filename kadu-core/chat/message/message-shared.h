/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_SHARED_H
#define MESSAGE_SHARED_H

#include <QtCore/QSharedData>

#include "chat/message/message.h"
#include "buddies/buddy.h"

class Chat;

class MessageShared : public QObject, public QSharedData
{
	Q_OBJECT
	Q_DISABLE_COPY(MessageShared)

	Chat MyChat;
	Contact Sender;
	QString Content;
	QDateTime ReceiveDate;
	QDateTime SendDate;
	Message::Status MyStatus;
	Message::Type MyType;
	int Id;

public:
	MessageShared(Chat chat = 0, Message::Type type = Message::TypeUnknown, Contact sender = Contact::null);
	virtual ~MessageShared();

	Chat  chat() const { return MyChat; }
	MessageShared & setChat(Chat chat);

	Contact sender() const { return Sender; }
	MessageShared & setSender(Contact sender);

	QString content() const { return Content; }
	MessageShared & setContent(const QString &content);

	QDateTime receiveDate() const { return ReceiveDate; }
	MessageShared & setReceiveDate(QDateTime receiveDate);

	QDateTime sendDate() const { return SendDate; }
	MessageShared & setSendDate(QDateTime sendDate);

	Message::Status status() const { return MyStatus; }
	MessageShared & setStatus(Message::Status status);

	Message::Type type() const { return MyType; }
	MessageShared & setType(Message::Type type);

	int id() const { return Id; }
	MessageShared & setId(int id);

signals:
	void statusChanged(Message::Status);

};

#endif // MESSAGE_SHARED_H
