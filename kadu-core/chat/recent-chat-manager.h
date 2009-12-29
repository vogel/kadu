/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RECENT_CHAT_MANAGER_H
#define RECENT_CHAT_MANAGER_H

#include <QtCore/QObject>

#include "storage/storable-object.h"

class Chat;

class KADUAPI RecentChatManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(RecentChatManager)

	static RecentChatManager * Instance;

	QList<Chat> RecentChats;

	RecentChatManager();
	virtual ~RecentChatManager();

	virtual void load();

	void removeRecentChat(Chat chat);

public:
	static RecentChatManager * instance();

	virtual QString storageNodeName() { return QLatin1String("RecentChats"); }
	virtual StorableObject* storageParent() { return 0; }

	virtual void store();

	void addRecentChat(Chat chat);
	QList<Chat> recentChats();

signals:
	void recentChatAboutToBeAdded(Chat chat);
	void recentChatAdded(Chat chat);
	void recentChatAboutToBeRemoved(Chat chat);
	void recentChatRemoved(Chat chat);

};

#include "chat/chat.h" // for MOC

#endif // RECENT_CHAT_MANAGER_H
