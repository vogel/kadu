/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>

#include "contacts/contact.h"

class Chat;

class Message : public QObject
{
	Q_OBJECT

public:
	enum Status
	{
		Unknown,
		Received,
		Sent,
		SentWaitingForAck,
		SentAckReceived
	};

private:
	Chat *MyChat;
	Contact Sender;
	QString Content;
	QDateTime ReceiveDate;
	QDateTime SendDate;
	Status MyStatus;

public:
	Message(Chat *chat = 0, Contact sender = Contact::null);
	Message(const Message &copyMe);

	void operator = (const Message &copyMe);

	Chat * chat() const { return MyChat; }
	Message & setChat(Chat *chat);

	Contact sender() const { return Sender; }
	Message & setSender(Contact sender);

	QString content() const { return Content; }
	Message & setContent(const QString &content);

	QDateTime receiveDate() const { return ReceiveDate; }
	Message & setReceiveDate(QDateTime receiveDate);

	QDateTime sendDate() const { return SendDate; }
	Message & setSendDate(QDateTime sendDate);

	Status status() const { return MyStatus; }
	Message & setStatus(Status status);

signals:
	void statusChanged(Status);

};

#endif // MESSAGE_H
