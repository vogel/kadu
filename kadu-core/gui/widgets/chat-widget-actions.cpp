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
#include "buddies/ignored-helper.h"
#include "contacts/contact.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/message-box.h"
#include "gui/windows/search-window.h"

#include "custom-input.h"
#include "debug.h"
#include "emoticons.h"

#include "chat-widget-actions.h"

void disableEmptyTextBox(Action *action)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(!chatEditBox->inputBox()->toPlainText().isEmpty());
}

void checkBlocking(Action *action)
{
	Account account = AccountManager::instance()->defaultAccount();
	BuddySet contacts = action->buddies();

	if (contacts.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	bool on = false;
	foreach (const Buddy buddy, action->buddies())
		if (buddy.isBlocked(account))
		{
			on = true;
			break;
		}
	action->setChecked(on);
}

void checkIgnoreUser(Action *action)
{
	BuddySet contacts = action->buddies();

	if (contacts.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setChecked(IgnoredHelper::isIgnored(action->buddies()));
}

ChatWidgetActions::ChatWidgetActions(QObject *parent) : QObject(parent)
{
	AutoSend = new ActionDescription(0,
		ActionDescription::TypeChat, "autoSendAction",
		this, SLOT(autoSendActionActivated(QAction *, bool)),
		"AutoSendMessage", tr("%1 sends message").arg(config_file.readEntry("ShortCuts", "chat_newline")), true
	);
	connect(AutoSend, SIGNAL(actionCreated(Action *)), this, SLOT(autoSendActionCreated(Action *)));

	ClearChat = new ActionDescription(0,
		ActionDescription::TypeChat, "clearChatAction",
		this, SLOT(clearActionActivated(QAction *, bool)),
		"ClearChat", tr("Clear messages in chat window")
	);

	InsertImage = new ActionDescription(0,
		ActionDescription::TypeChat, "insertImageAction",
		this, SLOT(insertImageActionActivated(QAction *, bool)),
		"ChooseImage", tr("Insert image")
	);

	Bold = new ActionDescription(0,
		ActionDescription::TypeChat, "boldAction",
		this, SLOT(boldActionActivated(QAction *, bool)),
		"Bold", tr("Bold"), true
	);

	Italic = new ActionDescription(0,
		ActionDescription::TypeChat, "italicAction",
		this, SLOT(italicActionActivated(QAction *, bool)),
		"Italic", tr("Italic"), true
	);

	Underline = new ActionDescription(0,
		ActionDescription::TypeChat, "underlineAction",
		this, SLOT(underlineActionActivated(QAction *, bool)),
		"Underline", tr("Underline"), true
	);

	Send = new ActionDescription(0,
		ActionDescription::TypeChat, "sendAction",
		this, SLOT(sendActionActivated(QAction *, bool)),
		"SendMessage", tr("&Send"), false, QString::null,
		disableEmptyTextBox
	);
	connect(Send, SIGNAL(actionCreated(Action *)), this, SLOT(sendActionCreated(Action *)));

	Whois = new ActionDescription(0,
		ActionDescription::TypeChat, "whoisAction",
		this, SLOT(whoisActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search this user in directory"), false, QString::null,
		disableNoGaduUle
	);

	IgnoreUser = new ActionDescription(0,
		ActionDescription::TypeUser, "ignoreUserAction",
		this, SLOT(ignoreUserActionActivated(QAction *, bool)),
		"Ignore", tr("Ignore user"), true, QString::null,
		checkIgnoreUser
	);

	BlockUser = new ActionDescription(0,
		ActionDescription::TypeUser, "blockUserAction",
		this, SLOT(blockUserActionActivated(QAction *, bool)),
		"Blocking", tr("Block user"), true, QString::null,
		checkBlocking
	);

	OpenChat = new ActionDescription(0,
		ActionDescription::TypeUser, "chatAction",
		this, SLOT(openChatActionActivated(QAction *, bool)),
		"OpenChat", tr("&Chat"), false, QString::null,
		disableContainsSelfUles
	);

	OpenChatWith = new ActionDescription(0,
		ActionDescription::TypeGlobal, "openChatWithAction",
		this, SLOT(openChatActionActivated(QAction *, bool)),
		"OpenChat", tr("Open chat with...")
	);
	OpenChatWith->setShortcut("kadu_openchatwith", Qt::ApplicationShortcut);

	InsertEmoticon = new ActionDescription(0,
		ActionDescription::TypeChat, "insertEmoticonAction",
		this, SLOT(insertEmoticonActionActivated(QAction *, bool)),
		"ChooseEmoticon", tr("Insert emoticon")
	);
	connect(InsertEmoticon, SIGNAL(actionCreated(Action *)), this, SLOT(insertEmoticonActionCreated(Action *)));

	ColorSelector = new ActionDescription(0,
		ActionDescription::TypeChat, "colorAction",
		this, SLOT(colorSelectorActionActivated(QAction *, bool)),
		"Black", tr("Change color")
	);

	BuddiesListViewMenuManager::instance()->addActionDescription(OpenChat);
}

ChatWidgetActions::~ChatWidgetActions()
{
}

void ChatWidgetActions::configurationUpdated()
{
	autoSendActionCheck();
}

void ChatWidgetActions::autoSendActionCreated(Action *action)
{
	action->setChecked(config_file.readBoolEntry("Chat", "AutoSend"));
}

void ChatWidgetActions::sendActionCreated(Action *action)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	connect(chatEditBox->inputBox(), SIGNAL(textChanged()), action, SLOT(checkState()));

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;
}

void ChatWidgetActions::insertEmoticonActionCreated(Action *action)
{
	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_NONE)
	{
		action->setToolTip(tr("Insert emoticon - enable in configuration"));
		action->setEnabled(false);
	}
}

void ChatWidgetActions::autoSendActionCheck()
{
 	bool check = config_file.readBoolEntry("Chat", "AutoSend");
 	foreach (Action *action, AutoSend->actions())
 		action->setChecked(check);
}

void ChatWidgetActions::autoSendActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	config_file.writeEntry("Chat", "AutoSend", toggled);
 	chatEditBox->setAutoSend(toggled);
	autoSendActionCheck();
}

void ChatWidgetActions::clearActionActivated(QAction *sender, bool toggled)
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

void ChatWidgetActions::insertImageActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->openInsertImageDialog();
}

void ChatWidgetActions::boldActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontWeight(toggled ? QFont::Bold : QFont::Normal);

	kdebugf2();
}

void ChatWidgetActions::italicActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontItalic(toggled);

	kdebugf2();
}

void ChatWidgetActions::underlineActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontUnderline(toggled);

	kdebugf2();
}

void ChatWidgetActions::sendActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->sendMessage();

	kdebugf2();
}

void ChatWidgetActions::whoisActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
	{
		(new SearchWindow(Core::instance()->kaduWindow()))->show();
		return;
	}

	Buddy buddy = window->buddy();
	if (buddy.isNull())
		return;

	SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(), buddy);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void ChatWidgetActions::ignoreUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account account = AccountManager::instance()->defaultAccount();
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	BuddySet contacts = window->buddies();
	if (contacts.count() > 0)
	{
		bool ContainsBad = false;
		foreach (Buddy buddy, contacts)
		{
			QString uid = buddy.accountData(account)->id();
			if (!account.protocolHandler()->validateUserID(uid))
			{
				ContainsBad = true;
				break;
			}
		}

		Chat *chat = account.protocolHandler()->findChat(contacts);
		if (chat && !ContainsBad)
		{
			if (IgnoredHelper::isIgnored(contacts))
				IgnoredHelper::setIgnored(contacts, false);
			else
			{
				IgnoredHelper::setIgnored(contacts);
				ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
				if (chatWidget)
				{
					ChatContainer *container = dynamic_cast<ChatContainer *>(chatWidget->window());
					if (container)
						container->closeChatWidget(chatWidget);
				}
			}
// TODO: 0.6.6
// 			kadu->userbox()->refresh();

			foreach (Action *action, IgnoreUser->actions())
			{
				if (action->buddies() == contacts)
					action->setChecked(IgnoredHelper::isIgnored(contacts));
			}

		}
	}
	kdebugf2();
}

void ChatWidgetActions::blockUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account account = AccountManager::instance()->defaultAccount();
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	BuddySet contacts = window->buddies();
	if (contacts.count() > 0)
	{
		bool on = true;
		bool blocked_anonymous = false; // true, if we blocked at least one anonymous user

		BuddySet copy = contacts;

		foreach(Buddy user, copy)
			if (user.accountData(account) == 0 || !user.isBlocked(account))
			{
				on = false;
				break;
			}

		foreach(Buddy user, copy)
		{
			QString uid = user.accountData(account)->id();
			if (account.protocolHandler()->validateUserID(uid) && user.isBlocked(account) != !on)
			{
//TODO: 0.6.6
/// 				user.setProtocolData("Gadu", "Blocking", !on);
				if ((!on) && (!blocked_anonymous) && user.isAnonymous())
					blocked_anonymous = true;
			}
		}

		Chat *chat = account.protocolHandler()->findChat(contacts);
		if (chat && !on) // if we were blocking, we also close the chat (and show info if blocked anonymous)
		{
			if (blocked_anonymous)
				MessageBox::msg(tr("Anonymous users will be unblocked after restarting Kadu"), false, "Information", Core::instance()->kaduWindow());

			ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
			if (chatWidget)
			{
				ChatContainer *container = dynamic_cast<ChatContainer *>(chatWidget->window());
				if (container)
					container->closeChatWidget(chatWidget);
			}
		}

// TODO: 0.6.6
// 		userlist->writeToConfig();

		foreach (Action *action, BlockUser->actions())
		{
			if (action->buddies() == contacts)
				action->setChecked(!on);
		}
	}
	kdebugf2();
}

void ChatWidgetActions::openChatActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	BuddySet contacts = window->buddies();
	if (contacts.count() > 0)
	{
		Chat *chat = (*contacts.begin()).prefferedAccount().protocolHandler()->findChat(contacts);
		if (chat)
			ChatWidgetManager::instance()->openChatWidget(chat, true);
	}

	kdebugf2();
}

void ChatWidgetActions::colorSelectorActionActivated(QAction *sender, bool toggled)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.size() == 0)
		return;

	chatEditBox->openColorSelector(widgets[widgets.size() - 1]);
}

void ChatWidgetActions::insertEmoticonActionActivated(QAction *sender, bool toggled)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.size() == 0)
		return;

	chatEditBox->openEmoticonSelector(widgets[widgets.size() - 1]);
}
