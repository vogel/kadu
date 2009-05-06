/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "contacts/ignored-helper.h"

#include "core/core.h"

#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/widgets/custom_input.h"

#include "gui/windows/chat_window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

#include "action.h"
#include "activate.h"
#include "chat/chat_message.h"
#include "config_file.h"
#include "debug.h"
#include "icons-manager.h"
#include "message_box.h"
#include "misc/misc.h"
#include "pending_msgs.h"
#include "search.h"
#include "xml_config_file.h"

#include "chat-widget-manager.h"

ChatWidgetManager * ChatWidgetManager::Instance = 0;

ChatWidgetManager * ChatWidgetManager::instance()
{
	if (0 == Instance)
		Instance = new ChatWidgetManager();

	return Instance;
}

ChatWidgetManager::ChatWidgetManager()
{
	kdebugf();

	ChatMessage::registerParserTags();
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	connect(Core::instance(), SIGNAL(messageReceived(Chat *, Contact, const QString &)),
			this, SLOT(messageReceived(Chat *, Contact, const QString &)));
	connect(Core::instance(), SIGNAL(disconnected()), this, SLOT(refreshTitles()));

	refreshTitlesTimer.setSingleShot(true);

	connect(&refreshTitlesTimer, SIGNAL(timeout()), this, SLOT(refreshTitles()));
// TODO: 0.6.6
// 	connect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshTitlesLater()));

	Actions = new ChatWidgetActions(this);

	configurationUpdated();

	kdebugf2();
}

void ChatWidgetManager::closeAllWindows()
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		saveOpenedWindows();

	foreach (ChatWidget *chat, Chats)
	{
		ChatWindow *window = dynamic_cast<ChatWindow *>(chat->parent());
		if (window)
			delete window;
	}

	Chats.clear();

	kdebugf2();
}

void ChatWidgetManager::loadOpenedWindows()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->findElement(root_elem, "ChatWindows");
	if (!chats_elem.isNull())
	{
		for (QDomNode win = chats_elem.firstChild(); !win.isNull(); win = win.nextSibling())
		{
			const QDomElement &window_elem = win.toElement();
			if (window_elem.isNull())
				continue;
			if (window_elem.tagName() != "Window")
				continue;
			QDomElement protocolNode = xml_config_file->getNode(window_elem, "Protocol", XmlConfigFile::ModeFind);
			QString protocolId = protocolNode.text();
			QDomElement windowIdNode = xml_config_file->getNode(window_elem, "WindowId", XmlConfigFile::ModeFind);
			QString accountId = windowIdNode.text();

			ContactSet contacts;
			for (QDomNode contact = window_elem.firstChild(); !contact.isNull(); contact = contact.nextSibling())
			{
				const QDomElement &contact_elem = contact.toElement();
				if (contact_elem.isNull())
					continue;
				if (contact_elem.tagName() != "Contact")
					continue;
				QString uuid = contact_elem.text();
				contacts.insert(ContactManager::instance()->byUuid(uuid));
			}

			// TODO 0.6.6: fix
			Account *defaultAccount = AccountManager::instance()->defaultAccount();
			if (defaultAccount)
			{
				Chat *chat = defaultAccount->protocol()->findChat(contacts);
				if (chat)
					openChatWidget(chat);
			}
			else
				kdebugm(KDEBUG_WARNING, "protocol %s/%s not found!\n",
					       qPrintable(protocolId), qPrintable(accountId));
		}
	}
	kdebugf2();
}

void ChatWidgetManager::saveOpenedWindows()
{
	// TODO: 0.6.6 saveOpenedChats?

/*
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->accessElement(root_elem, "ChatWindows");
	xml_config_file->removeChildren(chats_elem);
	foreach (ChatWidget *chat, ChatWidgets)
	{
		if (!chat->currentProtocol() || !chat->currentProtocol()->protocolFactory())
			continue;

		QDomElement windowNode = xml_config_file->getNode(chats_elem,
			"Window", XmlConfigFile::ModeCreate);
		// TODO 0.6.6 - gadu raus!

		xml_config_file->createTextNode(windowNode, "Protocol", chat->currentProtocol()->protocolFactory()->displayName());
		xml_config_file->createTextNode(windowNode, "WindowId", "Gadu");

		foreach(Contact contact, chat->contacts())
			xml_config_file->createTextNode(windowNode, "Contact", contact.uuid());
	}
	kdebugf2();*/
}

ChatWidgetManager::~ChatWidgetManager()
{
	kdebugf();

	ChatMessage::unregisterParserTags();

	disconnect(Core::instance(), SIGNAL(messageReceived(Chat *, Contact , const QString &)),
			this, SLOT(messageReceived(Chat *, Contact , const QString &)));

	disconnect(&refreshTitlesTimer, SIGNAL(timeout()), this, SLOT(refreshTitles()));
// TODO: 0.6.6
//	disconnect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshTitlesLater()));

	closeAllWindows();

#ifdef DEBUG_ENABLED
	// for valgrind
	QStringList chatActions;
	chatActions << "autoSendAction" << "clearChatAction"
				<< "insertEmoticonAction" << "whoisAction" << "insertImageAction"
				<< "ignoreUserAction" << "blockUserAction" << "boldAction"
				<< "italicAction" << "underlineAction" << "colorAction"
				<< "sendAction" << "chatAction" << "openChatWithAction";
	foreach(const QString &act, chatActions)
	{
		ActionDescription *a = KaduActions[act];
		delete a;
	}
#endif

	kdebugf2();
}

void ChatWidgetManager::openChatWith()
{
	(new OpenChatWith(Core::instance()->kaduWindow()))->show();
}

void ChatWidgetManager::insertEmoticonActionEnabled()
{
 	foreach (KaduAction *action, Actions->insertEmoticon()->actions())
	{
		if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_NONE)
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

const QHash<Chat *, ChatWidget *> & ChatWidgetManager::chats() const
{
	return Chats;
}

const QList<Chat *> ChatWidgetManager::closedChats() const
{
	return ClosedChats;
}

void ChatWidgetManager::registerChatWidget(ChatWidget *chat)
{
	kdebugf();

	ClosedChats.removeOne(chat->chat());
	Chats.insert(chat->chat(), chat);
}

void ChatWidgetManager::unregisterChatWidget(ChatWidget *chat)
{
	kdebugf();

	if (!Chats.contains(chat->chat()))
		return;

	if (chat->countMessages())
	{
		ClosedChats.prepend(chat->chat());
		if (ClosedChats.count() > 10)
			ClosedChats.pop_back();
	}

	emit chatWidgetDestroying(chat);
	Chats.remove(chat->chat());
}

void ChatWidgetManager::refreshTitlesLater()
{
	refreshTitlesTimer.start(0);
}

void ChatWidgetManager::refreshTitles()
{
	kdebugf();
 	foreach (ChatWidget *chat, Chats)
		chat->refreshTitle();
	emit chatWidgetTitlesUpdated();
	kdebugf2();
}

void ChatWidgetManager::refreshTitlesForUser(Contact contact)
{
	kdebugf();
// TOOD: 0.6.6 need implementation in Chat signal: titleChanged()
/* 	foreach(ChatWidget *chat, ChatWidgets)
 		if (chat->contacts().contains(contact))
 			chat->refreshTitle();*/
	kdebugf2();
}

ChatWidget * ChatWidgetManager::byChat(Chat *chat, bool create) const
{
	return Chats.contains(chat)
		? Chats[chat]
		: 0;
}

void ChatWidgetManager::activateChatWidget(ChatWidget *chatWidget, bool forceActivate)
{
	QWidget *win = chatWidget->window();
	kdebugm(KDEBUG_INFO, "parent: %p\n", win);

//TODO 0.6.6:
	if (forceActivate)
	{
		activateWindow(win->winId()); /* Dorr: this sometimes doesn't work */
		win->setWindowState(Qt::WindowActive);
	}

	win->raise();
	chatWidget->makeActive();
	emit chatWidgetOpen(chatWidget);
}

ChatWidget * ChatWidgetManager::openChatWidget(Chat *chat, bool forceActivate)
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
	{
		ChatWindow *window = new ChatWindow();
		chatWidget->setParent(window);
		chatWidget->show();
		window->setChatWidget(chatWidget);
		window->show();
	}
 	chatWidget->refreshTitle();

	connect(chatWidget, SIGNAL(messageSentAndConfirmed(ContactList, const QString &)),
		this, SIGNAL(messageSentAndConfirmed(ContactList, const QString &)));

//TODO 0.6.6:
	if (forceActivate)
	{
		QWidget *win = chatWidget->window();
		activateWindow(win->winId());
	}

	emit chatWidgetCreated(chatWidget);
// TODO: remove, it is so stupid ...
	emit chatWidgetCreated(chatWidget, time(0));
	emit chatWidgetOpen(chatWidget);

	kdebugf2();

	return chatWidget;
}

void ChatWidgetManager::deletePendingMsgs(Chat *chat)
{
	kdebugf();
	for (int i = 0; i < pending.count(); ++i)
		if (pending[i].chat == chat)
		{
			pending.deleteMsg(i);
			--i;
		}

// TODO: 0.6.6
// 	UserBox::refreshAllLater();
	kdebugf2();
}

// TODO: make pending messages ChatMessages or something
ChatMessage *convertPendingToMessage(PendingMsgs::Element elem)
{
	kdebugf();

	QDateTime date;
	date.setTime_t(elem.time);

	ContactSet receivers;
	receivers.insert(Core::instance()->myself());
	ChatMessage *message = new ChatMessage(elem.chat->account(),
			elem.sender, receivers, elem.msg,
			TypeReceived, QDateTime::currentDateTime(), date);

	return message;
}

void ChatWidgetManager::openPendingMsgs(Chat *chat, bool forceActivate)
{
	kdebugf();

	QList<ChatMessage *> messages;
	PendingMsgs::Element elem;

	ChatWidget *chatWidget = openChatWidget(chat, forceActivate);
	if (!chatWidget)
		return;

	for (int i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if (elem.chat != chat)
			continue;
		messages.append(convertPendingToMessage(elem));
		pending.deleteMsg(i--);
	}

	if (messages.size())
	{
		// TODO: Lame API
		if (!chatWidget->countMessages())
			chatWidget->appendMessages(messages, true);
// TODO: 0.6.6
// 		UserBox::refreshAllLater();
	}

	kdebugf2();
}

void ChatWidgetManager::openPendingMsgs(bool forceActivate)
{
	kdebugf();

	if (pending.count())
		openPendingMsgs(pending[0].chat, forceActivate);

	kdebugf2();
}

void ChatWidgetManager::sendMessage(Chat *chat)
{
	kdebugf();

	for (int i = 0; i < pending.count(); ++i)
		if (pending[i].chat == chat)
		{
			openPendingMsgs(pending[i].chat);
			return;
		}

	if (chat)
		openChatWidget(chat, true);

	kdebugf2();
}

void ChatWidgetManager::configurationUpdated()
{
	kdebugf();

	insertEmoticonActionEnabled();

	kdebugf2();
}

void ChatWidgetManager::messageReceived(Chat *chat, Contact sender, const QString &message)
{
	kdebugf();
	ContactSet receipients = chat->contacts();
	Account *account = chat->account();
	time_t time = QDateTime::currentDateTime().toTime_t();

	ChatWidget *chatWidget = byChat(chat);
	if (chatWidget)
		chatWidget->newMessage(account, sender, receipients, message, time);
	else
	{
		if (config_file.readBoolEntry("General","AutoRaise"))
		{
			Core::instance()->kaduWindow()->showNormal();
			Core::instance()->kaduWindow()->setFocus();
		}

		if (config_file.readBoolEntry("Chat", "OpenChatOnMessage"))
		{
			// TODO: 0.6.6
			if (config_file.readBoolEntry("Chat", "OpenChatOnMessageWhenOnline") && false /*!Myself.status("Gadu").isOnline()*/)
			{
				pending.addMsg(chat, sender, message, time);
				return;
			}

			// TODO: it is lame
			openChatWidget(chat);
			chatWidget = byChat(chat);
			chatWidget->newMessage(account, sender, receipients, message, time);
		}
		else
			pending.addMsg(chat, sender, message, time);
	}

	kdebugf2();
}

ChatWidgetManager* chat_manager = 0;
