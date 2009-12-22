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

#include "buddies/buddy.h"
#include "chat/chat.h"

#include "exports.h"

class MessageShared;

class KADUAPI Message : public SharedBase<MessageShared>
{
	KaduSharedBaseClass(Message)

public:
	enum Status
	{
		StatusUnknown,
		StatusReceived,
		StatusSent,
		StatusDelivered,
		StatusWontDeliver
	};
	enum Type
	{
		TypeUnknown,
		TypeReceived,
		TypeSent,
		TypeSystem
	};

public:
	static Message create();
	static Message loadFromStorage(StoragePoint *messageStoragePoint);
	static Message null;

	Message();
	Message(MessageShared *data);
	Message(QObject *data);
	Message(const Message &copy);
	virtual ~Message();

	KaduSharedBase_Property(Chat, messageChat, MessageChat)
	KaduSharedBase_Property(Contact, messageSender, MessageSender)
	KaduSharedBase_Property(QString, content, Content)
	KaduSharedBase_Property(QDateTime, receiveDate, ReceiveDate)
	KaduSharedBase_Property(QDateTime, sendDate, SendDate)
	KaduSharedBase_Property(Message::Status, status, Status)
	KaduSharedBase_Property(Message::Type, type, Type)
	KaduSharedBase_PropertyBool(Pending)
	KaduSharedBase_Property(int, id, Id)

};

#endif // MESSAGE_H
