/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/message/message.h"
#include "chat/message/message-render-info.h"
#include "chat/message/pending-messages-manager.h"
#include "chat/chat-manager.h"
#include "chat/recent-chat-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "emoticons/emoticons-manager.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-container.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/windows/chat-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/open-chat-with/open-chat-with.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

#include "activate.h"
#include "debug.h"
#include "icons/icons-manager.h"
#include "misc/misc.h"
#include "search.h"

#include "chat-widget-manager.h"

ChatWidgetManager * ChatWidgetManager::Instance = 0;

ChatWidgetManager * ChatWidgetManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ChatWidgetManager();
		// Load configuration in constructor creates loop because Instance == 0
		Instance->ensureLoaded();
	}

	return Instance;
}

ChatWidgetManager::ChatWidgetManager()
{
	kdebugf();
	setState(StateNotLoaded);

	MessageRenderInfo::registerParserTags();

	connect(Core::instance(), SIGNAL(messageReceived(const Message &)),
			this, SLOT(messageReceived(const Message &)));
	connect(Core::instance(), SIGNAL(messageSent(const Message &)),
			this, SLOT(messageSent(const Message &)));

	Actions = new ChatWidgetActions(this);

	configurationUpdated();

	kdebugf2();
}


ChatWidgetManager::~ChatWidgetManager()
{
	kdebugf();

	MessageRenderInfo::unregisterParserTags();

	disconnect(Core::instance(), SIGNAL(messageReceived(const Message &)),
			this, SLOT(messageReceived(const Message &)));

	closeAllWindows();


	kdebugf2();
}

StorableObject * ChatWidgetManager::storageParent()
{
	return 0;
}

QString ChatWidgetManager::storageNodeName()
{
	return QLatin1String("ChatWindows");
}

QString ChatWidgetManager::storageItemNodeName()
{
	return QLatin1String("Chat");
}

void ChatWidgetManager::closeAllWindows()
{
	kdebugf();

	ensureStored();

	QHash<Chat, ChatWidget *>::iterator i = Chats.begin();
	while (i != Chats.end())
	{
		ChatWindow *window = qobject_cast<ChatWindow *>(i.value()->window());
		if (window)
		{
			i = Chats.erase(i);
			delete window;
			continue;
		}
		++i;
	}

	kdebugf2();
}

void ChatWidgetManager::load()
{
	if (!isValidStorage())
		return;

	StorableStringList::load();

	foreach (const QString &uuid, content())
	{
		QUuid chatId(uuid);

		if (chatId.isNull())
			continue;

		openChat(ChatManager::instance()->byUuid(chatId));
	}
}

void ChatWidgetManager::store()
{
	if (!isValidStorage())
		return;

	StringList.clear();

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
	{
		for (QHash<Chat , ChatWidget *>::const_iterator it = Chats.constBegin(), end = Chats.constEnd(); it != end; ++it)
		{
			Protocol *protocolHandler = it.key().chatAccount().protocolHandler();
			if (!protocolHandler || !protocolHandler->protocolFactory() || !qobject_cast<ChatWindow *>(it.value()->window()))
					continue;

			StringList.append(it.key().uuid().toString());
		}
	}

	StorableStringList::store();
}

void ChatWidgetManager::insertEmoticonActionEnabled()
{
	QString toolTip;
	bool enabled;

	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EmoticonsStyleNone)
	{
		toolTip =  tr("Insert emoticon - enable in configuration");
		enabled = false;
	}
	else
	{
		toolTip = tr("Insert emoticon");
		enabled = true;
	}

 	foreach (Action *action, Actions->insertEmoticon()->actions())
	{
		action->setToolTip(toolTip);
		action->setEnabled(enabled);
	}
}

const QHash<Chat , ChatWidget *> & ChatWidgetManager::chats() const
{
	return Chats;
}

ChatWidget * ChatWidgetManager::byChat(const Chat &chat) const
{
	return Chats.contains(chat)
			? Chats.value(chat)
			: 0;
}

void ChatWidgetManager::activateChatWidget(ChatWidget *chatwidget)
{
	chatwidget->activate();
	emit chatWidgetOpen(chatwidget);
}

ChatWidget * ChatWidgetManager::createChatWidget(const Chat &chat)
{
	if (!chat)
		return 0;

	ChatWidget *chatWidget = new ChatWidget(chat);
	Chats.insert(chat, chatWidget);

	bool handled = false;
	emit handleNewChatWidget(chatWidget, handled);
	if (!handled)
		(new ChatWindow(chatWidget))->show();

	connect(chatWidget, SIGNAL(messageSentAndConfirmed(Chat , const QString &)),
		this, SIGNAL(messageSentAndConfirmed(Chat , const QString &)));
	connect(chatWidget, SIGNAL(destroyed()), this, SLOT(chatWidgetDestroyed()));

//	if (chatWidget->chat().contacts().count() == 1)
//	{
//		Contact contact = chatWidget->chat().contacts().toContact();
//		BuddyPreferredManager::instance()->updatePreferred(contact.ownerBuddy());
//	}

	emit chatWidgetCreated(chatWidget);

	return chatWidget;
}

void ChatWidgetManager::chatWidgetDestroyed()
{
	ChatWidget *chatWidget = qobject_cast<ChatWidget *>(sender());
	if (!chatWidget)
		return;

	if (!Chats.contains(chatWidget->chat()))
		return;

	Chats.remove(chatWidget->chat());

//	if (chatwidget->chat().contacts().count() == 1)
//	{
//		Contact contact = chatwidget->chat().contacts().toContact();
//		BuddyPreferredManager::instance()->updatePreferred(contact.ownerBuddy());
//	}

	emit chatWidgetDestroying(chatWidget);
}

ChatWidget * ChatWidgetManager::openChatWidget(const Chat &chat)
{
	if (!chat)
		return 0;

	ChatWidget *chatWidget = byChat(chat);
	if (!chatWidget)
		chatWidget = createChatWidget(chat);
	if (chatWidget)
		activateChatWidget(chatWidget);

	return chatWidget;
}

QList<MessageRenderInfo *> ChatWidgetManager::readPendingMessages(const Chat &chat)
{
	const Chat &aggregateChat = AggregateChatManager::instance()->aggregateChat(chat);
	const QVector<Message> &pendingMessages = PendingMessagesManager::instance()->pendingMessagesForChat(
	            aggregateChat ? aggregateChat : chat);

	QList<MessageRenderInfo *> messages;
	foreach (Message message, pendingMessages)
	{
		messages.append(new MessageRenderInfo(message));
		message.setPending(false);
		PendingMessagesManager::instance()->removeItem(message);
	}

	return messages;
}

void ChatWidgetManager::openChat(const Chat &chat)
{
	if (!chat)
		return;

	ChatWidget *chatWidget = openChatWidget(chat);
	if (!chatWidget)
		return;

	const QList<MessageRenderInfo *> &messages = readPendingMessages(chat);
	if (!messages.isEmpty())
		// TODO: Lame API
		if (0 == chatWidget->countMessages())
			chatWidget->appendMessages(messages, true);
}

void ChatWidgetManager::openPendingMessages()
{
	const Message &message = PendingMessagesManager::instance()->firstPendingMessage();
	openChat(message.messageChat());
}

void ChatWidgetManager::closeChat(const Chat &chat)
{
	ChatWidget *chatWidget = byChat(chat);
	if (chatWidget)
	{
		ChatWidgetContainer *container = dynamic_cast<ChatWidgetContainer *>(chatWidget->window());
		if (container)
			container->closeChatWidget(chatWidget);
	}
}

void ChatWidgetManager::closeAllChats(const Buddy &buddy)
{
	foreach (const Contact &contact, buddy.contacts())
	{
		Chat chat = ChatManager::instance()->findChat(ContactSet(contact), false);
		if (chat)
			closeChat(chat);
	}
}

void ChatWidgetManager::configurationUpdated()
{
	kdebugf();

	OpenChatOnMessage = config_file.readBoolEntry("Chat", "OpenChatOnMessage");
	AutoRaise = config_file.readBoolEntry("General","AutoRaise");
	OpenChatOnMessageWhenOnline = config_file.readBoolEntry("Chat", "OpenChatOnMessageWhenOnline");

	insertEmoticonActionEnabled();

	kdebugf2();
}

// TODO 0.10.0:, move to core or somewhere else
void ChatWidgetManager::messageReceived(const Message &message)
{
	kdebugf();

	Chat chat = message.messageChat();
	ChatWidget *chatWidget = byChat(chat);
	if (chatWidget)
	{
		MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
		chatWidget->newMessage(messageRenderInfo);
	}
	else
	{
		if (AutoRaise)
			_activateWindow(Core::instance()->kaduWindow());

		if (OpenChatOnMessage)
		{
			Protocol *handler = message.messageChat().chatAccount().protocolHandler();
			if (OpenChatOnMessageWhenOnline && (!handler || (handler->status().group() != StatusTypeGroupOnline)))
			{
				qApp->alert(Core::instance()->kaduWindow());
				message.setPending(true);
				PendingMessagesManager::instance()->addItem(message);
				return;
			}

			// TODO: it is lame
			openChat(chat);
			chatWidget = byChat(chat);

			MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
			chatWidget->newMessage(messageRenderInfo);
		}
		else
		{
			qApp->alert(Core::instance()->kaduWindow());
			message.setPending(true);
			PendingMessagesManager::instance()->addItem(message);
		}
	}

	kdebugf2();
}

void ChatWidgetManager::messageSent(const Message &message)
{
	Chat chat = message.messageChat();
	ChatWidget *chatWidget = byChat(chat);
	MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);

	if (!chatWidget)
	{
		openChat(chat);
		chatWidget = byChat(chat);
	}

	if (!chatWidget)
		return;

	chatWidget->appendMessage(messageRenderInfo);
}
