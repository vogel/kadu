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

#include "chat/chat-shared.h"
#include "storage/shared-base.h"
#include "exports.h"

class QPixmap;

class Account;
class BuddySet;
class ChatDetails;
class ChatType;
class StoragePoint;

class KADUAPI Chat : public SharedBase<ChatShared>
{
	Chat(bool null);

public:
	static Chat create();
	static Chat loadFromStorage(StoragePoint *storage);
	static Chat null;

	Chat();
	Chat(ChatShared *data);
	Chat(QObject *data);
	Chat(const Chat &copy);
	virtual ~Chat();

	Chat & operator = (const Chat &copy);

	void refreshTitle();

	BuddySet buddies() const;
	QString name() const;

	KaduSharedBase_Property(ChatDetails *, details, Details)
	KaduSharedBase_Property(Account, chatAccount, ChatAccount)
	KaduSharedBase_Property(QString, type, Type)
	KaduSharedBase_Property(QString, title, Title)
	KaduSharedBase_Property(QPixmap, icon, Icon)

};

Q_DECLARE_METATYPE(Chat )

#endif // CHAT_H
