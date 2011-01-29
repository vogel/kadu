/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "chat/message/message.h"
#include "chat/message/message-render-info.h"
#include "chat/message/message-shared.h"
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
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/chat-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/open-chat-with/open-chat-with.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

#include "activate.h"
#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"
#include "search.h"

#include "chat-widget-manager.h"

ChatWidgetManager * ChatWidgetManager::Instance = 0;

ChatWidgetManager * ChatWidgetManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ChatWidgetManager();
		// TODO 0.6.6 Remove
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

	store();

	QHash<Chat, ChatWidget *>::iterator i = Chats.begin();
	while (i != Chats.end())
	{
		ChatWindow *window = dynamic_cast<ChatWindow *>(i.value()->window());
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

		Chat chat = ChatManager::instance()->byUuid(chatId);
		if (!chat)
			continue;

		openPendingMessages(chat, true);
	}
}

void ChatWidgetManager::store()
{
	if (!isValidStorage())
		return;

	StringList.clear();

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
	{
		// TODO: are all this conditions needed?
		foreach (const Chat &chat, Chats.keys())
		{
			if (chat.isNull() || chat.chatAccount().isNull() || !chat.chatAccount().protocolHandler()
				|| !chat.chatAccount().protocolHandler()->protocolFactory() || !dynamic_cast<ChatWindow *>(Chats.value(chat)->window()))
					continue;

			StringList.append(chat.uuid().toString());
		}
	}

	StorableStringList::store();
}

void ChatWidgetManager::insertEmoticonActionEnabled()
{
 	foreach (Action *action, Actions->insertEmoticon()->actions())
	{
		if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EmoticonsStyleNone)
		{
			action->setToolTip(tr("Insert emoticon - enable in configuration"));
			action->setEnabled(false);
		}
		else
		{
			action->setToolTip(tr("Insert emoticon"));
			action->setEnabled(true);
		}
	}
}

const QHash<Chat , ChatWidget *> & ChatWidgetManager::chats() const
{
	return Chats;
}

void ChatWidgetManager::registerChatWidget(ChatWidget *chatwidget)
{
	kdebugf();
	Chats.insert(chatwidget->chat(), chatwidget);
}

void ChatWidgetManager::unregisterChatWidget(ChatWidget *chatwidget)
{
	kdebugf();

	if (!Chats.contains(chatwidget->chat()))
		return;

	Chats.remove(chatwidget->chat());

	if (chatwidget->countMessages())
		RecentChatManager::instance()->addRecentChat(chatwidget->chat());

	if (chatwidget->chat().contacts().count() == 1)
	{
		Contact contact = chatwidget->chat().contacts().toContact();
		BuddyPreferredManager::instance()->updatePreferred(contact.ownerBuddy());
	}

	emit chatWidgetDestroying(chatwidget);
}

ChatWidget * ChatWidgetManager::byChat(const Chat &chat, bool create) const
{
	Q_UNUSED(create)

	return Chats.contains(chat)
		? Chats[chat]
		: 0;
}

void ChatWidgetManager::activateChatWidget(ChatWidget *chatwidget, bool forceActivate)
{
	// TODO: 0.6.6
	Q_UNUSED(forceActivate)

	QWidget *win = chatwidget->window();
	Q_UNUSED(win) // only in debug mode

	kdebugm(KDEBUG_INFO, "parent: %p\n", win);
	chatwidget->makeActive();
	emit chatWidgetOpen(chatwidget, true);
}

ChatWidget * ChatWidgetManager::openChatWidget(const Chat &chat, bool forceActivate)
{
	kdebugf();

	ChatWidget *chatWidget = byChat(chat);
	if (chatWidget)
	{
		activateChatWidget(chatWidget, forceActivate);
		return chatWidget;
	}

	chatWidget = new ChatWidget(chat);

	bool handled = false;
	emit handleNewChatWidget(chatWidget, handled);
	if (!handled)
		(new ChatWindow(chatWidget))->show();

	connect(chatWidget, SIGNAL(messageSentAndConfirmed(Chat , const QString &)),
		this, SIGNAL(messageSentAndConfirmed(Chat , const QString &)));

	if (forceActivate) //TODO 0.6.6:
	{
		chatWidget->makeActive();
	}

	if (chatWidget->chat().contacts().count() == 1)
	{
		Contact contact = chatWidget->chat().contacts().toContact();
		BuddyPreferredManager::instance()->updatePreferred(contact.ownerBuddy());
	}

	emit chatWidgetCreated(chatWidget);
// TODO: remove, it is so stupid ...
	emit chatWidgetCreated(chatWidget, time(0));
	emit chatWidgetOpen(chatWidget, forceActivate);

	kdebugf2();

	return chatWidget;
}

void ChatWidgetManager::deletePendingMessages(const Chat &chat)
{
	kdebugf();

	QList<Message> messages = PendingMessagesManager::instance()->pendingMessagesForChat(chat);
	foreach (Message message, messages)
	{
		message.setPending(false);
		PendingMessagesManager::instance()->removeItem(message);
	}

	kdebugf2();
}

void ChatWidgetManager::openPendingMessages(const Chat &chat, bool forceActivate)
{
	kdebugf();

	if (!chat)
		return;

	QList<MessageRenderInfo *> messages;

	ChatWidget *chatWidget = openChatWidget(chat, forceActivate);
	if (!chatWidget)
		return;

	QList<Message> pendingMessages = PendingMessagesManager::instance()->pendingMessagesForChat(chat);
	foreach (Message message, pendingMessages)
	{
		messages.append(new MessageRenderInfo(message));
		message.setPending(false);
		PendingMessagesManager::instance()->removeItem(message);
	}

	if (messages.size())
	{
		// TODO: Lame API
		if (!chatWidget->countMessages())
			chatWidget->appendMessages(messages, true);
	}

	kdebugf2();
}

void ChatWidgetManager::openPendingMessages(bool forceActivate)
{
	kdebugf();

	Message message = PendingMessagesManager::instance()->firstPendingMessage();
	if (message)
		openPendingMessages(message.messageChat(), forceActivate);

	kdebugf2();
}

void ChatWidgetManager::sendMessage(const Chat &chat)
{
	kdebugf();

	if (PendingMessagesManager::instance()->hasPendingMessagesForChat(chat))
	{
		openPendingMessages(chat);
		return;
	}

	if (chat)
		openChatWidget(chat, true);

	kdebugf2();
}

void ChatWidgetManager::closeChat(const Chat &chat)
{
	ChatWidget *chatWidget = byChat(chat);
	if (chatWidget)
	{
		ChatContainer *container = dynamic_cast<ChatContainer *>(chatWidget->window());
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

	insertEmoticonActionEnabled();

	kdebugf2();
}

// TODO: 0.8, move to core or somewhere else
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
		if (config_file.readBoolEntry("General","AutoRaise"))
		{
			Core::instance()->kaduWindow()->showNormal();
			Core::instance()->kaduWindow()->setFocus();
		}

		if (config_file.readBoolEntry("Chat", "OpenChatOnMessage"))
		{
			Protocol *handler = message.messageChat().chatAccount().protocolHandler();
			if (config_file.readBoolEntry("Chat", "OpenChatOnMessageWhenOnline") && (!handler || (handler->status().group() != "Online")))
			{
				message.setPending(true);
				PendingMessagesManager::instance()->addItem(message);
				return;
			}

			// TODO: it is lame
			openChatWidget(chat);
			chatWidget = byChat(chat);

			MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
			chatWidget->newMessage(messageRenderInfo);
		}
		else
		{
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
		openChatWidget(chat);
		chatWidget = byChat(chat);
	}

	if (!chatWidget)
		return;

	chatWidget->appendMessage(messageRenderInfo);
}
