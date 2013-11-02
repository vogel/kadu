/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>

#include "chat/buddy-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-actions.h"
#include "gui/widgets/chat-widget/chat-widget-container.h"
#include "gui/widgets/chat-widget/chat-widget-factory.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/chat-window.h"
#include "gui/windows/kadu-window.h"
#include "icons/icons-manager.h"
#include "message/message-manager.h"
#include "message/message-render-info.h"
#include "message/unread-message-repository.h"
#include "protocols/protocol-factory.h"
#include "services/notification-service.h"
#include "activate.h"

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
	setState(StateNotLoaded);

	MessageRenderInfo::registerParserTags();

	connect(MessageManager::instance(), SIGNAL(messageReceived(const Message &)),
			this, SLOT(messageReceived(const Message &)));
	connect(MessageManager::instance(), SIGNAL(messageSent(const Message &)),
			this, SLOT(messageSent(const Message &)));

	Actions = new ChatWidgetActions(this);

	configurationUpdated();
}


ChatWidgetManager::~ChatWidgetManager()
{
	MessageRenderInfo::unregisterParserTags();

	disconnect(MessageManager::instance(), 0, this, 0);

	closeAllWindows();
}

void ChatWidgetManager::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	CurrentChatWidgetRepository = chatWidgetRepository;

	connect(CurrentChatWidgetRepository.data(), SIGNAL(chatWidgetCreated(ChatWidget*)),
			this, SLOT(chatWidgetCreated(ChatWidget*)));
	connect(CurrentChatWidgetRepository.data(), SIGNAL(chatWidgetDestroyed(ChatWidget*)),
			this, SLOT(chatWidgetDestroyed(ChatWidget*)));
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
	ensureStored();

	if (!CurrentChatWidgetRepository)
		return;

	auto chatWidget = CurrentChatWidgetRepository.data()->widgets().begin();
	while (chatWidget != CurrentChatWidgetRepository.data()->widgets().end())
	{
		delete chatWidget.value();
		chatWidget = CurrentChatWidgetRepository.data()->widgets().begin();
	}
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

		byChat(ChatManager::instance()->byUuid(chatId), true);
	}
}

void ChatWidgetManager::store()
{
	if (!isValidStorage())
		return;

	StringList.clear();

	if (CurrentChatWidgetRepository && config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
	{
		auto end = CurrentChatWidgetRepository.data()->widgets().constEnd();
		for (auto it = CurrentChatWidgetRepository.data()->widgets().constBegin(); it != end; ++it)
		{
			Protocol *protocolHandler = it.key().chatAccount().protocolHandler();
			if (!protocolHandler || !protocolHandler->protocolFactory() || !qobject_cast<ChatWindow *>(it.value()->window()))
					continue;

			StringList.append(it.key().uuid().toString());
		}
	}

	StorableStringList::store();
}

ChatWidget * ChatWidgetManager::byChat(const Chat &chat, const bool create)
{
	if (!chat || !CurrentChatWidgetRepository)
		return 0;

	if (CurrentChatWidgetRepository.data()->hasWidgetForChat(chat) || create)
		return CurrentChatWidgetRepository.data()->widgetForChat(chat);

	return 0;
}

void ChatWidgetManager::chatWidgetCreated(ChatWidget *chatWidget)
{
	// We need to append unread messages before chat widget container could mark them as read.
	const QList<Message> &messages = loadUnreadMessages(chatWidget->chat());
	chatWidget->appendMessages(messages);

	bool handled = false;
	emit handleNewChatWidget(chatWidget, handled);
	if (!handled)
	{
		ChatWindow *chatWindow = new ChatWindow(chatWidget);
		chatWidget->setContainer(chatWindow);
		chatWindow->show();
	}
}

QList<Message> ChatWidgetManager::loadUnreadMessages(const Chat &chat)
{
	const Chat &buddyChat = BuddyChatManager::instance()->buddyChat(chat);
	const Chat &unreadChat = buddyChat ? buddyChat : chat;
	const QList<Message> &unreadMessages = Core::instance()->unreadMessageRepository()->chatUnreadMessages(unreadChat);

	foreach (const Message &message, unreadMessages)
	{
		message.setPending(false);
		message.data()->removeFromStorage();
	}

	return unreadMessages;
}

void ChatWidgetManager::closeChat(const Chat &chat)
{
	ChatWidget * const chatWidget = byChat(chat, false);
	if (chatWidget && chatWidget->container())
		chatWidget->container()->closeChatWidget(chatWidget);
}

void ChatWidgetManager::closeAllChats(const Buddy &buddy)
{
	foreach (const Contact &contact, buddy.contacts())
	{
		const Chat &chat = ChatTypeContact::findChat(contact, ActionReturnNull);
		if (chat)
			closeChat(chat);
	}
}

void ChatWidgetManager::configurationUpdated()
{
	OpenChatOnMessage = config_file.readBoolEntry("Chat", "OpenChatOnMessage");
	OpenChatOnMessageOnlyWhenOnline = config_file.readBoolEntry("Chat", "OpenChatOnMessageWhenOnline");
}

bool ChatWidgetManager::shouldOpenChatWidget(const Message &message)
{
	if (!OpenChatOnMessage)
		return false;

	if ((OpenChatOnMessage || OpenChatOnMessageOnlyWhenOnline) && Core::instance()->notificationService()->silentMode())
		return false;

	const Protocol * const handler = message.messageChat().chatAccount().protocolHandler();
	if (!handler)
		return false;

	if (!OpenChatOnMessageOnlyWhenOnline)
		return true;

	return StatusTypeGroupOnline == handler->status().group();
}

void ChatWidgetManager::messageReceived(const Message &message)
{
	const Chat &chat = message.messageChat();
	ChatWidget *alreadyOpenedChatWidget = byChat(chat, false);

	if (alreadyOpenedChatWidget)
	{
		alreadyOpenedChatWidget->appendMessage(message);
		return;
	}

	if (!shouldOpenChatWidget(message))
	{
		qApp->alert(Core::instance()->kaduWindow());
		return;
	}

	byChat(chat, true);
}

void ChatWidgetManager::messageSent(const Message &message)
{
	const Chat &chat = message.messageChat();
	ChatWidget * const chatWidget = byChat(chat, false);
	if (!chatWidget)
		return;

	chatWidget->appendMessage(message);
}

#include "moc_chat-widget-manager.cpp"
