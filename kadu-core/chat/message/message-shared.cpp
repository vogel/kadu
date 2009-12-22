/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "message-shared.h"

MessageShared * MessageShared::loadFromStorage(StoragePoint *messageStoragePoint)
{
	MessageShared *result = new MessageShared();
	result->setStorage(messageStoragePoint);
	return result;
}

MessageShared::MessageShared(QUuid uuid) :
		Shared(uuid), Status(Message::StatusUnknown), Type(Message::TypeUnknown)
{
}

MessageShared::~MessageShared()
{
}

StorableObject * MessageShared::storageParent()
{
	return 0; // TODO: use PendingMessageManager or STH
}

QString MessageShared::storageNodeName()
{
	return QLatin1String("Message");
}

void MessageShared::load()
{
	// TODO: write real implementation
	Shared::load();
}

void MessageShared::store()
{
	// TODO: write real implementation
	Shared::store();
}

void MessageShared::emitUpdated()
{
	emit updated();
}

void MessageShared::setStatus(Message::Status status)
{
	if (status != Status)
	{
		Status = status;
		emit statusChanged(Status);
	}
}
