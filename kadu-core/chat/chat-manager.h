/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_MANAGER
#define CHAT_MANAGER

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "configuration/storable-object.h"

#include "chat.h"

class Account;
class XmlConfigFile;

class ChatManager : public QObject, public StorableObject, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatManager)

	static ChatManager * Instance;

	QMap<Account *, QList<Chat *> > Chats;

	ChatManager();

	void load(Account *account);
	void store(Account *account);

protected:
	virtual StoragePoint * createStoragePoint();

	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	static ChatManager * instance();

	unsigned int count() { return Chats.count(); }

	virtual void store();

	void addChat(Chat *chat);
	void removeChat(Chat *chat);
	QList<Chat *> chatsForAccount(Account *account);

signals:
	void chatAboutToBeAdded(Chat *chat);
	void chatAdded(Chat *chat);
	void chatAboutToBeRemoved(Chat *chat);
	void chatRemoved(Chat *chat);

};

#endif // CHAT_MANAGER
