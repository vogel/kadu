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

#include "gui/widgets/chat_edit_box.h"
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
#include "icons_manager.h"
#include "message_box.h"
#include "misc/misc.h"
#include "pending_msgs.h"
#include "search.h"
#include "xml_config_file.h"

#include "chat-widget-manager.h"

void disableEmptyTextBox(KaduAction *action)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(!chatEditBox->inputBox()->toPlainText().isEmpty());
}

void checkBlocking(KaduAction *action)
{
	Account *account = AccountManager::instance()->defaultAccount();
	ContactSet contacts = action->contacts();

	if (contacts.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}
	
	bool on = false;
	foreach (const Contact contact, action->contacts())
		if (contact.isBlocked(account))
		{
			on = true;
			break;
		}
	action->setChecked(on);
}

void checkIgnoreUser(KaduAction *action)
{
	ContactSet contacts = action->contacts();

	if (contacts.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setChecked(IgnoredHelper::isIgnored(action->contacts()));
}

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

	connect(Core::instance(), SIGNAL(messageReceived(Chat *, Contact, const QString &)),
			this, SLOT(messageReceived(Chat *, Contact, const QString &)));
	connect(Core::instance(), SIGNAL(disconnected()), this, SLOT(refreshTitles()));

	refreshTitlesTimer.setSingleShot(true);

	autoSendActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "autoSendAction",
		this, SLOT(autoSendActionActivated(QAction *, bool)),
		"AutoSendMessage", tr("%1 sends message").arg(config_file.readEntry("ShortCuts", "chat_newline")), true
	);
	connect(autoSendActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(autoSendActionCreated(KaduAction *)));

	clearChatActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "clearChatAction",
		this, SLOT(clearActionActivated(QAction *, bool)),
		"ClearChat", tr("Clear messages in chat window")
	);

	insertImageActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "insertImageAction",
		this, SLOT(insertImageActionActivated(QAction *, bool)),
		"ChooseImage", tr("Insert image")
	);

	boldActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "boldAction",
		this, SLOT(boldActionActivated(QAction *, bool)),
		"Bold", tr("Bold"), true
	);

	italicActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "italicAction",
		this, SLOT(italicActionActivated(QAction *, bool)),
		"Italic", tr("Italic"), true
	);

	underlineActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "underlineAction",
		this, SLOT(underlineActionActivated(QAction *, bool)),
		"Underline", tr("Underline"), true
	);

	sendActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "sendAction",
		this, SLOT(sendActionActivated(QAction *, bool)),
		"SendMessage", tr("&Send"), false, QString::null,
		disableEmptyTextBox
	);
	connect(sendActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(sendActionCreated(KaduAction *)));

	whoisActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "whoisAction",
		this, SLOT(whoisActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search this user in directory"), false, QString::null,
		disableEmptyUles
	);

	ignoreUserActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "ignoreUserAction",
		this, SLOT(ignoreUserActionActivated(QAction *, bool)),
		"Ignore", tr("Ignore user"), true, QString::null,
		checkIgnoreUser
	);

	blockUserActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "blockUserAction",
		this, SLOT(blockUserActionActivated(QAction *, bool)),
		"Blocking", tr("Block user"), true, QString::null,
		checkBlocking
	);

	chatActionDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "chatAction",
		this, SLOT(chatActionActivated(QAction *, bool)),
		"OpenChat", tr("&Chat"), false, QString::null,
		disableContainsSelfUles
	);

	openChatWithActionDescription = new ActionDescription(0,
		ActionDescription::TypeGlobal, "openChatWithAction",
		this, SLOT(openChatWith()),
		"OpenChat", tr("Open chat with...")
	);
	openChatWithActionDescription->setShortcut("kadu_openchatwith", Qt::ApplicationShortcut);

	insertEmoticonActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "insertEmoticonAction",
		this, SLOT(insertEmoticonActionActivated(QAction *, bool)),
		"ChooseEmoticon", tr("Insert emoticon")
	);
	connect(insertEmoticonActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(insertEmoticonActionCreated(KaduAction *)));

	colorSelectorActionDescription = new ActionDescription(0,
		ActionDescription::TypeChat, "colorAction",
		this, SLOT(colorSelectorActionActivated(QAction *, bool)),
		"Black", tr("Change color")
	);

	ContactsListWidgetMenuManager::instance()->addActionDescription(chatActionDescription);
	ContactsListWidgetMenuManager::instance()->addSeparator();

	connect(&refreshTitlesTimer, SIGNAL(timeout()), this, SLOT(refreshTitles()));
// TODO: 0.6.6
// 	connect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshTitlesLater()));

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

void ChatWidgetManager::autoSendActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
	{
		chatWidget->setAutoSend(toggled);
		config_file.writeEntry("Chat", "AutoSend", toggled);
		autoSendActionCheck();
	}

	kdebugf2();
}

void ChatWidgetManager::clearActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->clearChatWindow();

	kdebugf2();
}

void ChatWidgetManager::insertImageActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->insertImage();

	kdebugf2();
}

void ChatWidgetManager::boldActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontWeight(toggled ? QFont::Bold : QFont::Normal);

	kdebugf2();
}

void ChatWidgetManager::italicActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontItalic(toggled);

	kdebugf2();
}

void ChatWidgetManager::underlineActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontUnderline(toggled);

	kdebugf2();
}

void ChatWidgetManager::sendActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	// TODO: split in two ?
	if (chatWidget)
		if (chatWidget->waitingForACK())
			chatWidget->cancelMessage();
		else
			chatWidget->sendMessage();

	kdebugf2();
}

void ChatWidgetManager::sendActionCreated(KaduAction *action)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	connect(chatEditBox->inputBox(), SIGNAL(textChanged()), action, SLOT(checkState()));

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	if (chatWidget->waitingForACK())
		chatWidget->changeSendToCancelSend();
}

void ChatWidgetManager::whoisActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
	{
		(new SearchDialog(Core::instance()->kaduWindow()))->show();
		return;
	}
	Account *defaultAccount = AccountManager::instance()->defaultAccount();
	ContactSet contacts = window->contacts();

	if (contacts.count() == 0)
		(new SearchDialog(Core::instance()->kaduWindow()))->show();
	else
	{
// 		if (contacts[0].accountData(AccountManager::instance()->defaultAccount()) != 0)
// 		{
			SearchDialog *sd = new SearchDialog(Core::instance()->kaduWindow()/*, contacts[0].accountData(AccountManager::instance()->defaultAccount())->id().toUInt()*/);
			sd->show();
			sd->firstSearch();
// 		}
	}

	kdebugf2();
}

void ChatWidgetManager::insertEmoticonActionCreated(KaduAction *action)
{
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_NONE)
	{
		action->setToolTip(tr("Insert emoticon - enable in configuration"));
		action->setEnabled(false);
	}
}

void ChatWidgetManager::insertEmoticonActionEnabled()
{
 	foreach (KaduAction *action, insertEmoticonActionDescription->actions())
	{
		if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_NONE)
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

void ChatWidgetManager::insertEmoticonActionActivated(QAction *sender, bool toggled)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
	{
		QList<QWidget *> widgets = sender->associatedWidgets();
		if (widgets.size() == 0)
			return;

		chatWidget->openEmoticonSelector(widgets[widgets.size() - 1]);
	}
}

void ChatWidgetManager::colorSelectorActionActivated(QAction *sender, bool toggled)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
	{
		QList<QWidget *> widgets = sender->associatedWidgets();
		if (widgets.size() == 0)
			return;

		chatWidget->changeColor(widgets[widgets.size() - 1]);
	}
}

void ChatWidgetManager::ignoreUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	if (contacts.count() > 0)
	{
		bool ContainsBad = false;
		foreach(Contact contact, contacts)
		{
			QString uid = contact.accountData(account)->id();
			if (!account->protocol()->validateUserID(uid))
			{
				ContainsBad = true;
				break;
			}
		}

		Chat *chat = account->protocol()->findChat(contacts);
		if (chat && !ContainsBad)
		{
			if (IgnoredHelper::isIgnored(contacts))
				IgnoredHelper::setIgnored(contacts, false);
			else
			{
				IgnoredHelper::setIgnored(contacts);
				ChatWidget *chatWidget = byChat(chat);
				if (chatWidget)
				{
					ChatContainer *container = dynamic_cast<ChatContainer *>(chatWidget->window());
					if (container)
						container->closeChatWidget(chatWidget);
				}
			}
// TODO: 0.6.6
// 			kadu->userbox()->refresh();

			foreach (KaduAction *action, ignoreUserActionDescription->actions())
			{
				if (action->contacts() == contacts)
					action->setChecked(IgnoredHelper::isIgnored(contacts));
			}

		}
	}
	kdebugf2();
}

void ChatWidgetManager::blockUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	if (contacts.count() > 0)
	{
		bool on = true;
		bool blocked_anonymous = false; // true, if we blocked at least one anonymous user

		ContactSet copy = contacts;

		foreach(Contact user, copy)
			if (user.accountData(account) == 0 || !user.isBlocked(account))
			{
				on = false;
				break;
			}

		foreach(Contact user, copy)
		{
			QString uid = user.accountData(account)->id();
			if (account->protocol()->validateUserID(uid) && user.isBlocked(account) != !on)
			{
//TODO: 0.6.6
/// 				user.setProtocolData("Gadu", "Blocking", !on);
				if ((!on) && (!blocked_anonymous) && user.isAnonymous())
					blocked_anonymous = true;
			}
		}

		Chat *chat = account->protocol()->findChat(contacts);
		if (chat && !on) // if we were blocking, we also close the chat (and show info if blocked anonymous)
		{
			if (blocked_anonymous)
				MessageBox::msg(tr("Anonymous users will be unblocked after restarting Kadu"), false, "Information", Core::instance()->kaduWindow());

			ChatWidget *chatWidget = byChat(chat);
			if (chatWidget)
			{
				ChatContainer *container = dynamic_cast<ChatContainer *>(chatWidget->window());
				if (container)
					container->closeChatWidget(chatWidget);
			}
		}

// TODO: 0.6.5
// 		userlist->writeToConfig();

		foreach (KaduAction *action, blockUserActionDescription->actions())
		{
			if (action->contacts() == contacts)
				action->setChecked(!on);
		}
	}
	kdebugf2();
}

void ChatWidgetManager::chatActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	if (contacts.count() > 0)
	{
		Chat *chat = account->protocol()->findChat(contacts);
		if (chat)
			openChatWidget(chat, true);
	}

	kdebugf2();
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

void ChatWidgetManager::deletePendingMsgs(ContactSet contacts)
{
	kdebugf();
	for (int i = 0; i < pending.count(); ++i)
		if (pending[i].contacts == contacts)
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
	ChatMessage *message = new ChatMessage(AccountManager::instance()->defaultAccount(),
			*elem.contacts.begin(), receivers, elem.msg,
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
		if (!(elem.contacts == chat->contacts()))
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
// TODO: 0.6.6 fix that
// 	if (pending.count())
// 		openPendingMsgs(pending[0].contacts, forceActivate);

	kdebugf2();
}

void ChatWidgetManager::sendMessage(Chat *chat)
{
	kdebugf();
// TODO: 0.6.6
// 	Account *defaultAccount = AccountManager::instance()->defaultAccount();
// 	if (!defaultAccount)
// 		return;
// 
// 	for (int i = 0; i < pending.count(); ++i)
// 		if (pending[i].contacts.contains(contact))
// 		{
// 			openPendingMsgs(pending[i].contacts);
// 			return;
// 		}

	if (chat)
		openChatWidget(chat, true);

	kdebugf2();
}

void ChatWidgetManager::closeModule()
{
	kdebugf();

	ChatMessage::unregisterParserTags();

	kdebugf2();
}

void ChatWidgetManager::initModule()
{
	kdebugf();

	ChatMessage::registerParserTags();
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	kdebugf2();
}

void ChatWidgetManager::autoSendActionCreated(KaduAction *action)
{
	action->setChecked(config_file.readBoolEntry("Chat", "AutoSend"));
}

void ChatWidgetManager::autoSendActionCheck()
{
 	bool check = config_file.readBoolEntry("Chat", "AutoSend");
 	foreach (KaduAction *action, autoSendActionDescription->actions())
 		action->setChecked(check);
}

void ChatWidgetManager::configurationUpdated()
{
	kdebugf();

	autoSendActionCheck();
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
				pending.addMsg(account, sender, receipients, message, time);
				return;
			}

			// TODO: it is lame
			openChatWidget(chat);
			chatWidget = byChat(chat);
			chatWidget->newMessage(account, sender, receipients, message, time);
		}
		else
			pending.addMsg(account, sender, receipients, message, time);
	}

	kdebugf2();
}

ChatWidgetManager* chat_manager = 0;
