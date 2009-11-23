/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtGui/QPixmap>

#include "accounts/account.h"
#include "chat/type/chat-type-aware-object.h"
#include "configuration/uuid-storable-object.h"
#include "buddies/buddy-set.h"

#include "exports.h"

class ChatDetails;
class ChatType;

class KADUAPI Chat : public QObject, public UuidStorableObject, ChatTypeAwareObject
{
	Q_OBJECT

	ChatDetails *Details;

	Account CurrentAccount;
	QUuid Uuid;
	QString Title;
	QPixmap Icon;
	QString Type;

protected:
	virtual void chatTypeRegistered(ChatType *chatType);
	virtual void chatTypeUnregistered(ChatType *chatType);

public:
	static Chat * loadFromStorage(StoragePoint *conferenceStoragePoint);

	explicit Chat(StoragePoint *storage);
	explicit Chat(Account parentAccount, QUuid uuid = QUuid());
	virtual ~Chat();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	ChatType * type() const;
	BuddySet buddies() const;
	QString name() const;

	void setDetails(ChatDetails *details) { Details = details; }
	ChatDetails *details() { return Details; }

	Account account() { return CurrentAccount; }
	void setTitle(const QString &newTitle);
	QString title() { return Title; }
	QPixmap icon() { return Icon; }

public slots:
	void refreshTitle();

signals:
	void titleChanged(Chat *chat, const QString &newTitle);

};

Q_DECLARE_METATYPE(Chat *)

#endif // CHAT_H
