/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_MANAGER_H
#define CHAT_MANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "manager/manager.h"

#include "chat.h"

#include "exports.h"

class Account;
class BuddyList;
class XmlConfigFile;

class KADUAPI ChatManager : public QObject, public Manager<Chat>
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatManager)

	static ChatManager * Instance;

	ChatManager();
	virtual ~ChatManager();

private slots:
	void chatTypeLoaded();
	void chatTypeUnloaded();

protected:
	virtual QString configurationNodeName() { return QLatin1String("Chats"); }
	virtual QString configurationNodeItemName() { return QLatin1String("Chat"); }

	virtual void itemAdded(Chat item);
	virtual void itemRemoved(Chat item);

	virtual void itemAboutToBeRegistered(Chat item);
	virtual void itemRegisterd(Chat item);
	virtual void itemAboutToBeUnregisterd(Chat item);
	virtual void itemUnregistered(Chat item);


public:
	static ChatManager * instance();

signals:
	void chatAboutToBeAdded(Chat chat);
	void chatAdded(Chat chat);
	void chatAboutToBeRemoved(Chat chat);
	void chatRemoved(Chat chat);

};

#endif // CHAT_MANAGER_H
