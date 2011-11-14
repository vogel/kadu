/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CHAT_WIDGET_MANAGER_H
#define CHAT_WIDGET_MANAGER_H

#include <QtCore/QTimer>

#include "buddies/buddy-list.h"
#include "chat/message/message.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-widget.h"
#include "storage/storable-string-list.h"

#include "exports.h"

class ActionDescription;
class ChatWidgetActions;
class Protocol;

class KADUAPI ChatWidgetManager : public QObject, ConfigurationAwareObject, StorableStringList
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWidgetManager)

	static ChatWidgetManager *Instance;

	ChatWidgetActions *Actions;

	QHash<Chat , ChatWidget *> Chats;

	bool AutoRaise;
	bool OpenChatOnMessage;
	bool OpenChatOnMessageWhenOnline;

	ChatWidgetManager();
	virtual ~ChatWidgetManager();

	ChatWidget * createChatWidget(const Chat &chat);
	ChatWidget * openChatWidget(const Chat &chat);

	QList<MessageRenderInfo *> readPendingMessages(const Chat &chat);

private slots:
	void chatWidgetDestroyed();
	void messageSent(const Message &message);

protected:
	virtual void load();
	virtual void store();

	virtual void configurationUpdated();

public:
	static ChatWidgetManager * instance();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();
	virtual QString storageItemNodeName();

	ChatWidgetActions * actions() { return Actions; }

	const QHash<Chat, ChatWidget *> & chats() const;
	ChatWidget * byChat(const Chat &chat) const;

	void activateChatWidget(ChatWidget *chatwidget);

public slots:
	// for imagelink module
	void messageReceived(const Message &message);

	void openChat(const Chat &chat);

	void closeChat(const Chat &chat);
	void closeAllChats(const Buddy &buddy);
	void closeAllWindows();

signals:
	void handleNewChatWidget(ChatWidget *chatwidget, bool &handled);

	void chatWidgetCreated(ChatWidget *chatwidget);
	void chatWidgetActivated(ChatWidget *chatwidget);

	void chatWidgetDestroying(ChatWidget *chatwidget);

	void chatWidgetOpen(ChatWidget *chatwidget);
	void chatWidgetTitlesUpdated();

};

#endif // CHAT_WIDGET_MANAGER_H
