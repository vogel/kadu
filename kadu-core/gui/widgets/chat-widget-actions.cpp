/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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
#include "buddies/buddy-shared.h"
#include "buddies/ignored-helper.h"
#include "contacts/contact.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "chat/chat-manager.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/message-dialog.h"
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
	ContactSet contacts = action->contacts();

	if (contacts.toBuddySet().contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	bool on = false;
	foreach (const Contact &contact, contacts)
		if (contact.ownerBuddy().isBlocked())
		{
			on = true;
			break;
		}
	action->setChecked(on);
}

void checkIgnoreUser(Action *action)
{
	BuddySet buddies = action->contacts().toBuddySet();

	if (buddies.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setChecked(IgnoredHelper::isIgnored(buddies));
}

ChatWidgetActions::ChatWidgetActions(QObject *parent) : QObject(parent)
{
	AutoSend = new ActionDescription(0,
		ActionDescription::TypeChat, "autoSendAction",
		this, SLOT(autoSendActionActivated(QAction *, bool)),
		"kadu_icons/kadu-enter.png", "kadu_icons/kadu-enter.png", tr("%1 Sends Message").arg(config_file.readEntry("ShortCuts", "chat_newline")), true
	);
	connect(AutoSend, SIGNAL(actionCreated(Action *)), this, SLOT(autoSendActionCreated(Action *)));

	ClearChat = new ActionDescription(0,
		ActionDescription::TypeChat, "clearChatAction",
		this, SLOT(clearActionActivated(QAction *, bool)),
		"16x16/edit-clear.png", "16x16/edit-clear.png", tr("Clear Messages in Chat Window")
	);

	InsertImage = new ActionDescription(0,
		ActionDescription::TypeChat, "insertImageAction",
		this, SLOT(insertImageActionActivated(QAction *, bool)),
		"16x16/insert-image.png", "16x16/insert-image.png", tr("Insert Image")
	);

	Bold = new ActionDescription(0,
		ActionDescription::TypeChat, "boldAction",
		this, SLOT(boldActionActivated(QAction *, bool)),
		"16x16/format-text-bold.png", "16x16/format-text-bold.png", tr("Bold"), true
	);

	Italic = new ActionDescription(0,
		ActionDescription::TypeChat, "italicAction",
		this, SLOT(italicActionActivated(QAction *, bool)),
		"16x16/format-text-italic.png", "16x16/format-text-italic.png", tr("Italic"), true
	);

	Underline = new ActionDescription(0,
		ActionDescription::TypeChat, "underlineAction",
		this, SLOT(underlineActionActivated(QAction *, bool)),
		"16x16/format-text-underline.png", "16x16/format-text-underline.png", tr("Underline"), true
	);

	Send = new ActionDescription(0,
		ActionDescription::TypeChat, "sendAction",
		this, SLOT(sendActionActivated(QAction *, bool)),
		"16x16/go-next.png", "16x16/go-next.png", tr("&Send"), false, QString::null,
		disableEmptyTextBox
	);
	connect(Send, SIGNAL(actionCreated(Action *)), this, SLOT(sendActionCreated(Action *)));

	Whois = new ActionDescription(0,
		ActionDescription::TypeChat, "whoisAction",
		this, SLOT(whoisActionActivated(QAction *, bool)),
		"16x16/edit-find.png", "16x16/edit-find.png", tr("Search this User in Directory"), false, QString::null,
		disableNoGaduUle
	);

	IgnoreUser = new ActionDescription(0,
		ActionDescription::TypeUser, "ignoreUserAction",
		this, SLOT(ignoreUserActionActivated(QAction *, bool)),
		"kadu_icons/kadu-manageignored.png", "kadu_icons/kadu-manageignored.png", tr("Ignore Buddy"), true, QString::null,
		checkIgnoreUser
	);

	BlockUser = new ActionDescription(0,
		ActionDescription::TypeUser, "blockUserAction",
		this, SLOT(blockUserActionActivated(QAction *, bool)),
		"kadu_icons/kadu-blocking.png", "kadu_icons/kadu-blocking.png", tr("Block Buddy"), true, QString::null,
		checkBlocking
	);

	OpenChat = new ActionDescription(0,
		ActionDescription::TypeUser, "chatAction",
		this, SLOT(openChatActionActivated(QAction *, bool)),
		"16x16/internet-group-chat.png", "16x16/internet-group-chat.png", tr("&Chat with Contact"), false, QString::null,
		disableContainsSelfUles
	);

	OpenChatWith = new ActionDescription(0,
		ActionDescription::TypeGlobal, "openChatWithAction",
		this, SLOT(openChatActionActivated(QAction *, bool)),
		"16x16/internet-group-chat.png", "16x16/internet-group-chat.png", tr("Open Chat with...")
	);
	OpenChatWith->setShortcut("kadu_openchatwith", Qt::ApplicationShortcut);

	InsertEmoticon = new ActionDescription(0,
		ActionDescription::TypeChat, "insertEmoticonAction",
		this, SLOT(insertEmoticonActionActivated(QAction *, bool)),
		"16x16/face-smile.png", "16x16/face-smile.png", tr("Insert Emoticon")
	);
	connect(InsertEmoticon, SIGNAL(actionCreated(Action *)), this, SLOT(insertEmoticonActionCreated(Action *)));

	ColorSelector = new ActionDescription(0,
		ActionDescription::TypeChat, "colorAction",
		this, SLOT(colorSelectorActionActivated(QAction *, bool)),
		"kadu_icons/kadu-colors.png", "kadu_icons/kadu-colors.png", tr("Change Color")
	);

	BuddiesListViewMenuManager::instance()->addActionDescription(OpenChat);
	BuddiesListViewMenuManager::instance()->addSeparator();
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
	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EmoticonsStyleNone)
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
	Q_UNUSED(toggled)

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
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->openInsertImageDialog();
}

void ChatWidgetActions::boldActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontWeight(toggled ? QFont::Bold : QFont::Normal);

	kdebugf2();
}

void ChatWidgetActions::italicActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontItalic(toggled);

	kdebugf2();
}

void ChatWidgetActions::underlineActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontUnderline(toggled);

	kdebugf2();
}

void ChatWidgetActions::sendActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

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
	Q_UNUSED(toggled)

	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
	{
		(new SearchWindow(Core::instance()->kaduWindow()))->show();
		return;
	}

	Buddy buddy = window->contact().ownerBuddy();
	if (buddy.isNull())
		return;

	SearchWindow *sd = new SearchWindow(Core::instance()->kaduWindow(), buddy);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void ChatWidgetActions::ignoreUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();
	Account account = AccountManager::instance()->defaultAccount();
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	BuddySet buddies = contacts.toBuddySet();
	if (contacts.count() > 0)
	{
		if (IgnoredHelper::isIgnored(buddies))
			IgnoredHelper::setIgnored(buddies, false);
		else
		{
			IgnoredHelper::setIgnored(buddies);
			Chat chat = ChatManager::instance()->findChat(contacts);
			if (chat)
			{
				ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
				if (chatWidget)
				{
					ChatContainer *container = dynamic_cast<ChatContainer *>(chatWidget->window());
					if (container)
						container->closeChatWidget(chatWidget);
				}
			}
		}
		
		bool set = IgnoredHelper::isIgnored(buddies);
		foreach (Action *action, IgnoreUser->actions())
		{
			if (action->contacts().toBuddySet() == buddies)
				action->setChecked(set);
		}
	}
	kdebugf2();
}

void ChatWidgetActions::blockUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Account account = AccountManager::instance()->defaultAccount();
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	BuddySet buddies = contacts.toBuddySet();
	if (buddies.isEmpty())
		return;

	bool on = true;
	bool blocked_anonymous = false; // true, if we blocked at least one anonymous user

	foreach (Buddy user, buddies)
		if (!user.isBlocked())
		{
			on = false;
			break;
		}

	foreach (Buddy buddy, buddies)
		if (buddy.isAnonymous())
			blocked_anonymous = true;

	Chat chat = ChatManager::instance()->findChat(contacts);
	if (chat && !on) // if we were blocking, we also close the chat (and show info if blocked anonymous)
	{
// 		TODO: 0.6.6, is that true?
// 		if (blocked_anonymous)
// 			MessageDialog::msg(tr("Anonymous users will be unblocked after restarting Kadu"), false, "Information", Core::instance()->kaduWindow());

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
		if (action->contacts().toBuddySet() == buddies)
			action->setChecked(!on);
	}

	kdebugf2();
}

void ChatWidgetActions::openChatActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();
	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	Chat chat = ChatManager::instance()->findChat(window->contacts());
	if (chat)
		ChatWidgetManager::instance()->openChatWidget(chat, true);

	kdebugf2();
}

void ChatWidgetActions::colorSelectorActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

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
	Q_UNUSED(toggled)

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.size() == 0)
		return;

	chatEditBox->openEmoticonSelector(widgets[widgets.size() - 1]);
}
