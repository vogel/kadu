/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef CHAT_TYPE_MANAGER
#define CHAT_TYPE_MANAGER

#include <QtCore/QList>
#include <QtCore/QObject>

#include "chat/type/chat-type.h"
#include "exports.h"

class KADUAPI ChatTypeManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatTypeManager)

	static ChatTypeManager * Instance;

	QList<ChatType> ChatTypes;

	ChatTypeManager();
	virtual ~ChatTypeManager();

public:
	static ChatTypeManager * instance();

	void addChatType(ChatType chatType);
	void removeChatType(ChatType chatType);

	const QList<ChatType> & chatTypes() const { return ChatTypes; }
	QList<ChatType> chatTypes() { return ChatTypes; }

signals:
	void chatTypeAboutToBeAdded(ChatType chatType);
	void chatTypeAdded(ChatType chatType);
	void chatTypeAboutToBeRemoved(ChatType chatType);
	void chatTypeRemoved(ChatType chatType);

};

#endif // CHAT_TYPE_MANAGER
