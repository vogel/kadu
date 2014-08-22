/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include <QtWidgets/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "chat/chat-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/actions/chat/leave-chat-action.h"
#include "gui/actions/edit-talkable-action.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/toolbar.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/open-chat-with/open-chat-with.h"
#include "model/roles.h"
#include "debug.h"

#include "chat-widget-actions.h"

static void disableEmptyTextBoxOrNotConnected(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(chatEditBox->chatWidget()->chat().isConnected() && !chatEditBox->inputBox()->toPlainText().isEmpty());
}

static void disableEmptyMessages(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(0 != chatEditBox->chatWidget()->chatMessagesView()->countMessages());
}

static void disableNoChatImageService(Action *action)
{
	action->setEnabled(false);

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	Account account = action->context()->chat().chatAccount();
	if (!account)
		return;

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	action->setEnabled(protocol->chatImageService());
}

static void checkBlocking(Action *action)
{
	BuddySet buddies = action->context()->buddies();

	if (!buddies.count() || buddies.contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(!action->context()->roles().contains(ContactRole));

	bool on = false;
	foreach (const Buddy &buddy, buddies)
		if (buddy.isBlocked())
		{
			on = true;
			break;
		}
	action->setChecked(on);
}

// TODO: quickhack
static void disableNoGadu(Action *action)
{
	action->setEnabled(false);

	Chat chat = action->context()->chat();
	if (!chat)
		return;

	Protocol *protocol = chat.chatAccount().protocolHandler();
	if (!protocol)
		return;

	if (!protocol->protocolFactory())
		return;

	action->setEnabled(protocol->protocolFactory()->name() == "gadu");
}

ChatWidgetActions::ChatWidgetActions(QObject *parent) : QObject(parent)
{
	Actions::instance()->blockSignals();

	MoreActions = new ActionDescription(0,
		ActionDescription::TypeChat, "moreActionsAction",
		this, SLOT(moreActionsActionActivated(QAction *, bool)),
		KaduIcon(), tr("More..."), false
	);

	AutoSend = new ActionDescription(0,
		ActionDescription::TypeChat, "autoSendAction",
		this, SLOT(autoSendActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/enter"), tr("Return Sends Message"), true
	);
	connect(AutoSend, SIGNAL(actionCreated(Action *)), this, SLOT(autoSendActionCreated(Action *)));

	ClearChat = new ActionDescription(0,
		ActionDescription::TypeChat, "clearChatAction",
		this, SLOT(clearActionActivated(QAction *, bool)),
		KaduIcon("edit-clear"), tr("Clear Messages in Chat Window"), false,
		disableEmptyMessages
	);
	connect(ClearChat, SIGNAL(actionCreated(Action *)), this, SLOT(clearChatActionCreated(Action *)));

	InsertImage = new ActionDescription(0,
		ActionDescription::TypeChat, "insertImageAction",
		this, SLOT(insertImageActionActivated(QAction *, bool)),
		KaduIcon("insert-image"), tr("Insert Image"), false,
		disableNoChatImageService
	);

	Bold = new ActionDescription(0,
		ActionDescription::TypeChat, "boldAction",
		this, SLOT(boldActionActivated(QAction *, bool)),
		KaduIcon("format-text-bold"), tr("Bold"), true,
		disableNoGadu
	);

	Italic = new ActionDescription(0,
		ActionDescription::TypeChat, "italicAction",
		this, SLOT(italicActionActivated(QAction *, bool)),
		KaduIcon("format-text-italic"), tr("Italic"), true,
		disableNoGadu
	);

	Underline = new ActionDescription(0,
		ActionDescription::TypeChat, "underlineAction",
		this, SLOT(underlineActionActivated(QAction *, bool)),
		KaduIcon("format-text-underline"), tr("Underline"), true,
		disableNoGadu
	);

	Send = new ActionDescription(0,
		ActionDescription::TypeChat, "sendAction",
		this, SLOT(sendActionActivated(QAction *, bool)),
		KaduIcon("go-next"), tr("&Send"), false,
		disableEmptyTextBoxOrNotConnected
	);
	connect(Send, SIGNAL(actionCreated(Action *)), this, SLOT(sendActionCreated(Action *)));

	BlockUser = new ActionDescription(0,
		ActionDescription::TypeUser, "blockUserAction",
		this, SLOT(blockUserActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/block-buddy"), tr("Block Buddy"), true,
		checkBlocking
	);

	// The last ActionDescription of each type will send actionLoaded() signal.
	Actions::instance()->unblockSignals();

	OpenChat = new ActionDescription(0,
		ActionDescription::TypeUser, "chatAction",
		this, SLOT(openChatActionActivated(QAction *, bool)),
		KaduIcon("internet-group-chat"), tr("&Chat"), false,
		disableNoChat
	);

	MenuInventory::instance()
		->menu("buddy-list")
		->addAction(OpenChat, KaduMenu::SectionChat, 1000);

	OpenWith = new ActionDescription(0,
		ActionDescription::TypeGlobal, "openChatWithAction",
		this, SLOT(openChatWithActionActivated(QAction *, bool)),
		KaduIcon("internet-group-chat"), tr("Open Chat with...")
	);
	OpenWith->setShortcut("kadu_openchatwith", Qt::ApplicationShortcut);
/*
	ColorSelector = new ActionDescription(0,
		ActionDescription::TypeChat, "colorAction",
		this, SLOT(colorSelectorActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/change-color"), tr("Change Color")
	);*/

	EditTalkable = new EditTalkableAction(this);
	LeaveChat = new LeaveChatAction(this);
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
	action->setChecked(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "AutoSend"));
}

void ChatWidgetActions::clearChatActionCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	connect(chatEditBox->chatWidget()->chatMessagesView(), SIGNAL(messagesUpdated()), action, SLOT(checkState()));
}

void ChatWidgetActions::sendActionCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	connect(chatEditBox->inputBox(), SIGNAL(textChanged()), action, SLOT(checkState()));
	connect(chatEditBox->chatWidget()->chat(), SIGNAL(connected()), action, SLOT(checkState()));
	connect(chatEditBox->chatWidget()->chat(), SIGNAL(disconnected()), action, SLOT(checkState()));

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;
}

void ChatWidgetActions::autoSendActionCheck()
{
 	bool check = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "AutoSend");
 	foreach (Action *action, AutoSend->actions())
 		action->setChecked(check);
}

void ChatWidgetActions::autoSendActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	Application::instance()->configuration()->deprecatedApi()->writeEntry("Chat", "AutoSend", toggled);
	autoSendActionCheck();

	ChatConfigurationHolder::instance()->configurationUpdated();
}

void ChatWidgetActions::moreActionsActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.isEmpty())
		return;

	QWidget *widget = widgets.at(widgets.size() - 1);

	QWidget *parent = widget->parentWidget();
	while (0 != parent && 0 == qobject_cast<ToolBar *>(parent))
		parent = parent->parentWidget();
	ToolBar *toolbar = qobject_cast<ToolBar *>(parent);

	QMenu menu;
	QMenu *subMenu = new QMenu(tr("More"), &menu);

	foreach (const QString &actionName, Actions::instance()->keys())
	{
		if (toolbar && toolbar->windowHasAction(actionName, false))
			continue;

		ActionDescription *actionDescription = Actions::instance()->value(actionName);
		if (ActionDescription::TypeChat == actionDescription->type())
			menu.addAction(Actions::instance()->createAction(actionName, chatEditBox->actionContext(), chatEditBox));
		else if (ActionDescription::TypeUser == actionDescription->type())
			subMenu->addAction(Actions::instance()->createAction(actionName, chatEditBox->actionContext(), chatEditBox));
	}

	menu.addSeparator();
	menu.addMenu(subMenu);
	menu.exec(widget->mapToGlobal(QPoint(0, widget->height())));
}

void ChatWidgetActions::clearActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
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

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->openInsertImageDialog();
}

void ChatWidgetActions::boldActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontWeight(toggled ? QFont::Bold : QFont::Normal);

	kdebugf2();
}

void ChatWidgetActions::italicActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontItalic(toggled);

	kdebugf2();
}

void ChatWidgetActions::underlineActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	chatEditBox->inputBox()->setFontUnderline(toggled);

	kdebugf2();
}

void ChatWidgetActions::sendActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (chatWidget)
		chatWidget->sendMessage();

	kdebugf2();
}

void ChatWidgetActions::blockUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	BuddySet buddies = action->context()->buddies();
	if (buddies.isEmpty())
		return;

	bool on = false;

	foreach (const Buddy &buddy, buddies)
		if (!buddy.isBlocked())
		{
			on = true;
			break;
		}

	foreach (Buddy buddy, buddies)
	{
		buddy.setBlocked(on);
		// update actions
		updateBlockingActions(buddy);
	}

	kdebugf2();
}

void ChatWidgetActions::updateBlockingActions(Buddy buddy)
{
	QList<Contact> buddyContacts = buddy.contacts();

	foreach (Action *action, BlockUser->actions())
	{
		Contact contact = action->context()->contacts().toContact();
		if (contact)
			if (buddyContacts.contains(contact))
				action->setChecked(buddy.isBlocked());
	}
}

void ChatWidgetActions::openChatActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	Core::instance()->chatWidgetManager()->openChat(action->context()->chat(), OpenChatActivation::Activate);

	kdebugf2();
}

void ChatWidgetActions::openChatWithActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	OpenChatWith::instance()->show();

	kdebugf2();
}

void ChatWidgetActions::colorSelectorActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.isEmpty())
		return;

	chatEditBox->openColorSelector(widgets.at(widgets.size() - 1));
}

#include "moc_chat-widget-actions.cpp"
