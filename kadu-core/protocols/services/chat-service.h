/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include <QtCore/QObject>

#include "contacts/contact-list.h"

#include "chat/message/message.h"

class ChatService : public QObject
{
	Q_OBJECT

public:
	enum MessageStatus {
		StatusAcceptedDelivered,
		StatusAcceptedQueued,
		StatusRejectedBlocked,
		StatusRejectedBoxFull,
		StatusRejectedUnknown
	};

	ChatService(QObject *parent = 0)
		: QObject(parent) {}

public slots:
	virtual bool sendMessage(Contact user, const QString &messageContent);
	virtual bool sendMessage(ContactList users, const QString &messageContent);
	virtual bool sendMessage(Contact user, Message &message);
	virtual bool sendMessage(ContactList users, Message &message) = 0;

signals:
	void sendMessageFiltering(const ContactList users, QByteArray &msg, bool &stop);
	void messageStatusChanged(int messsageId, ChatService::MessageStatus status);
	void receivedMessageFilter(Account *account, Contact sender, ContactList receipients,
			const QString &message, time_t time, bool &ignore);
	void messageReceived(Account *account, Contact sender,
			ContactList receipients, const QString &message, time_t time);

};

#endif // CHAT_SERVICE_H
