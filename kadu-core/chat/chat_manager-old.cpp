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

#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"

#include "action.h"
#include "activate.h"
#include "chat_message.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc/misc.h"
#include "pending_msgs.h"
#include "search.h"
#include "xml_config_file.h"

#include "chat_manager-old.h"

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
	ContactList contacts = action->contacts();

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
	ContactList contacts = action->contacts();

	if (contacts.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setChecked(IgnoredHelper::isIgnored(action->contacts()));
}

ChatManagerOld::ChatManagerOld(QObject *parent)
	: QObject(parent), ChatWidgets(), ClosedChatUsers(), addons(), refreshTitlesTimer()
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

void ChatManagerOld::closeAllWindows()
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chat_manager->saveOpenedWindows();

	while (!ChatWidgets.empty())
	{
		ChatWidget *chat = ChatWidgets.first();
		ChatWindow *window = dynamic_cast<ChatWindow *>(chat->parent());
		if (window)
			delete window;
	}

	kdebugf2();
}

void ChatManagerOld::loadOpenedWindows()
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
			ContactList contacts;
			for (QDomNode contact = window_elem.firstChild(); !contact.isNull(); contact = contact.nextSibling())
			{
				const QDomElement &contact_elem = contact.toElement();
				if (contact_elem.isNull())
					continue;
				if (contact_elem.tagName() != "Contact")
					continue;
				QString uuid = contact_elem.text();
				contacts.append(ContactManager::instance()->byUuid(uuid));
			}

			// TODO 0.6.6: fix
			Account *defaultAccount = AccountManager::instance()->defaultAccount();
			if (defaultAccount)
				openChatWidget(defaultAccount, contacts);
			else
				kdebugm(KDEBUG_WARNING, "protocol %s/%s not found!\n",
					       qPrintable(protocolId), qPrintable(accountId));
		}
	}
	kdebugf2();
}

void ChatManagerOld::saveOpenedWindows()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->accessElement(root_elem, "ChatWindows");
	xml_config_file->removeChildren(chats_elem);
	foreach(ChatWidget *chat, ChatWidgets)
	{	
		QDomElement windowNode = xml_config_file->getNode(chats_elem,
			"Window", XmlConfigFile::ModeCreate);
		// TODO 0.6.6 - gadu raus!
		xml_config_file->createTextNode(windowNode, "Protocol", chat->currentProtocol()->protocolFactory()->displayName());
		xml_config_file->createTextNode(windowNode, "WindowId", "Gadu");

		foreach(Contact contact, chat->contacts())
			xml_config_file->createTextNode(windowNode, "Contact", contact.uuid());
	}
	kdebugf2();
}

ChatManagerOld::~ChatManagerOld()
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

void ChatManagerOld::openChatWith()
{
	(new OpenChatWith(Core::instance()->kaduWindow()))->show();
}

void ChatManagerOld::autoSendActionActivated(QAction *sender, bool toggled)
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

void ChatManagerOld::clearActionActivated(QAction *sender, bool toggled)
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

void ChatManagerOld::insertImageActionActivated(QAction *sender, bool toggled)
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

void ChatManagerOld::boldActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontWeight(toggled ? QFont::Bold : QFont::Normal);

	kdebugf2();
}

void ChatManagerOld::italicActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontItalic(toggled);

	kdebugf2();
}

void ChatManagerOld::underlineActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontUnderline(toggled);

	kdebugf2();
}

void ChatManagerOld::sendActionActivated(QAction *sender, bool toggled)
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

void ChatManagerOld::sendActionCreated(KaduAction *action)
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

void ChatManagerOld::whoisActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
	{
		(new SearchDialog(Core::instance()->kaduWindow()))->show();
		return;
	}
	Account *defaultAccount = AccountManager::instance()->defaultAccount();
	ContactList contacts = window->contacts();

	if (contacts.count() == 0)
		(new SearchDialog(Core::instance()->kaduWindow()))->show();
	else
	{
		if (contacts[0].accountData(AccountManager::instance()->defaultAccount()) != 0)
		{
			SearchDialog *sd = new SearchDialog(Core::instance()->kaduWindow()/*, contacts[0].accountData(AccountManager::instance()->defaultAccount())->id().toUInt()*/);
			sd->show();
			sd->firstSearch();
		}
	}

	kdebugf2();
}

void ChatManagerOld::insertEmoticonActionCreated(KaduAction *action)
{
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_NONE)
	{
		action->setToolTip(tr("Insert emoticon - enable in configuration"));
		action->setEnabled(false);
	}
}

void ChatManagerOld::insertEmoticonActionEnabled()
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

void ChatManagerOld::insertEmoticonActionActivated(QAction *sender, bool toggled)
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

void ChatManagerOld::colorSelectorActionActivated(QAction *sender, bool toggled)
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

void ChatManagerOld::ignoreUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
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

		if (!ContainsBad)
		{
			if (IgnoredHelper::isIgnored(contacts))
				IgnoredHelper::setIgnored(contacts, false);
			else
			{
				IgnoredHelper::setIgnored(contacts);
				ChatWidget *chat = findChatWidget(contacts);
				if (chat)
				{
					ChatContainer *container = dynamic_cast<ChatContainer *>(chat->window());
					if (container)
						container->closeChatWidget(chat);
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

void ChatManagerOld::blockUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	if (contacts.count() > 0)
	{
		bool on = true;
		bool blocked_anonymous = false; // true, if we blocked at least one anonymous user

		ContactList copy = contacts;

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

		if (!on) // if we were blocking, we also close the chat (and show info if blocked anonymous)
		{
			if (blocked_anonymous)
				MessageBox::msg(tr("Anonymous users will be unblocked after restarting Kadu"), false, "Information", Core::instance()->kaduWindow());

			ChatWidget *chat = findChatWidget(contacts);
			if (chat)
			{
				ChatContainer *container = dynamic_cast<ChatContainer *>(chat->window());
				if (container)
					container->closeChatWidget(chat);
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

void ChatManagerOld::chatActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	if (contacts.count() > 0)
		openChatWidget(account, contacts, true);

	kdebugf2();
}

const ChatList& ChatManagerOld::chats() const
{
	return ChatWidgets;
}

const QList<ContactList> ChatManagerOld::closedChatUsers() const
{
	return ClosedChatUsers;
}

int ChatManagerOld::registerChatWidget(ChatWidget *chat)
{
	kdebugf();
	ClosedChatUsers.removeOne(chat->contacts());
	ChatWidgets.append(chat);

	return ChatWidgets.count() - 1;
}

void ChatManagerOld::unregisterChatWidget(ChatWidget *chat)
{
	kdebugf();

	foreach (ChatWidget *curChat, ChatWidgets)
		if (curChat == chat)
		{
			if (chat->body->countMessages())
			{
				ClosedChatUsers.prepend(chat->contacts());
				if (ClosedChatUsers.count() > 10)
					ClosedChatUsers.pop_back();
			}

			emit chatWidgetDestroying(chat);
			ChatWidgets.removeAll(curChat);

			kdebugf2();
			return;
		}

	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "NOT found\n");
}

void ChatManagerOld::refreshTitlesLater()
{
	refreshTitlesTimer.start(0);
}

void ChatManagerOld::refreshTitles()
{
	kdebugf();
 	foreach(ChatWidget *chat, ChatWidgets)
		chat->refreshTitle();
	emit chatWidgetTitlesUpdated();
	kdebugf2();
}

void ChatManagerOld::refreshTitlesForUser(Contact contact)
{
	kdebugf();
 	foreach(ChatWidget *chat, ChatWidgets)
 		if (chat->contacts().contains(contact))
 			chat->refreshTitle();
	kdebugf2();
}

ChatWidget* ChatManagerOld::findChatWidget(ContactList contacts) const
{
	foreach(ChatWidget *chat, ChatWidgets)
		if (chat->contacts() == contacts)
			return chat;
	kdebugmf(KDEBUG_WARNING, "no such chat\n");
	return NULL;
}

ChatWidget * ChatManagerOld::chatWidgetForContactList(ContactList contacts)
{
	foreach (ChatWidget *chatWidget, ChatWidgets)
		if (chatWidget->contacts() == contacts)
			return chatWidget;

	return 0;
}

void ChatManagerOld::activateChatWidget(ChatWidget *chatWidget, bool forceActivate)
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

ChatWidget * ChatManagerOld::openChatWidget(Account *initialAccount, ContactList contacts, bool forceActivate)
{
	kdebugf();

	ChatWidget *chatWidget = chatWidgetForContactList(contacts);
	if (chatWidget)
	{
		activateChatWidget(chatWidget, forceActivate);
		return chatWidget;
	}

	Account *account = contacts[0].prefferedAccount();
	if (!account)
		return 0;

	chatWidget = new ChatWidget(account, contacts);

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

void ChatManagerOld::deletePendingMsgs(ContactList contacts)
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

	ContactList receivers;
	receivers << Core::instance()->myself();
	ChatMessage *message = new ChatMessage(AccountManager::instance()->defaultAccount(), elem.contacts[0], receivers, elem.msg,
			TypeReceived, QDateTime::currentDateTime(), date);

	return message;
}

void ChatManagerOld::openPendingMsgs(ContactList contacts, bool forceActivate)
{
	kdebugf();

	QList<ChatMessage *> messages;
	PendingMsgs::Element elem;
	Account *defaultAccount = AccountManager::instance()->defaultAccount();
	ChatWidget *chatWidget = openChatWidget(defaultAccount, contacts, forceActivate);

	if (!chatWidget)
		return;

	for (int i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if (!(elem.contacts == contacts))
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

void ChatManagerOld::openPendingMsgs(bool forceActivate)
{
	kdebugf();

	if (pending.count())
		openPendingMsgs(pending[0].contacts, forceActivate);

	kdebugf2();
}

void ChatManagerOld::sendMessage(Contact contact, ContactList selected_contacts)
{
	kdebugf();
	Account *defaultAccount = AccountManager::instance()->defaultAccount();
	for (int i = 0; i < pending.count(); ++i)
		if (pending[i].contacts.contains(contact))
		{
			openPendingMsgs(pending[i].contacts);
			return;
		}

	openChatWidget(defaultAccount, selected_contacts, true);

	kdebugf2();
}

QVariant& ChatManagerOld::chatWidgetProperty(ContactList contacts, const QString &name)
{
	kdebugf();

	QList<ChatInfo>::iterator addon;
	for (addon = addons.begin(); addon != addons.end(); ++addon)
	{
		if (contacts == (*addon).contacts)
		{
			kdebugf2();
			return (*addon).map[name];
		}
	}

	ChatInfo info;
	info.contacts = contacts;
	info.map[name] = QVariant();
	addons.push_front(info);
	kdebugmf(KDEBUG_FUNCTION_END, "end: %s NOT found\n", qPrintable(name));
	return addons[0].map[name];
}

void ChatManagerOld::setChatWidgetProperty(ContactList contacts, const QString &name, const QVariant &value)
{
	kdebugf();

	QList<ChatInfo>::iterator addon;
	for (addon = addons.begin(); addon != addons.end(); ++addon)
	{
		if (contacts == (*addon).contacts)
		{
			(*addon).map[name] = value;
			kdebugf2();
			return;
		}
	}
	ChatInfo info;
	info.contacts = contacts;
	info.map[name] = value;
	addons.push_front(info);
	kdebugf2();
}

void ChatManagerOld::closeModule()
{
	kdebugf();

	ChatMessage::unregisterParserTags();

	delete chat_manager;
	chat_manager = 0;
	kdebugf2();
}

void ChatManagerOld::initModule()
{
	kdebugf();

	ChatMessage::registerParserTags();
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
	chat_manager = new ChatManagerOld(Core::instance()->kaduWindow());

	kdebugf2();
}

void ChatManagerOld::autoSendActionCreated(KaduAction *action)
{
	action->setChecked(config_file.readBoolEntry("Chat", "AutoSend"));
}

void ChatManagerOld::autoSendActionCheck()
{
 	bool check = config_file.readBoolEntry("Chat", "AutoSend");
 	foreach (KaduAction *action, autoSendActionDescription->actions())
 		action->setChecked(check);
}

void ChatManagerOld::configurationUpdated()
{
	kdebugf();

	autoSendActionCheck();
	insertEmoticonActionEnabled();

	kdebugf2();
}

void ChatManagerOld::messageReceived(Chat *chat, Contact sender, const QString &message)
{
	kdebugf();
	ContactList receipients = chat->currentContacts();
	Account *account = chat->account();
	ContactList conference = receipients;
	conference << sender;
	time_t time = QDateTime::currentDateTime().toTime_t();

	ChatWidget *chatWidget = findChatWidget(conference);
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
			openChatWidget(account, conference);
			chatWidget = findChatWidget(conference);
			chatWidget->newMessage(account, sender, receipients, message, time);
		}
		else
			pending.addMsg(account, sender, receipients, message, time);
	}

	kdebugf2();
}

ChatManagerOld* chat_manager = 0;
