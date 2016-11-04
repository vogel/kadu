/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "core/myself.h"
#include "core/injected-factory.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/actions/chat/leave-chat-action.h"
#include "gui/actions/edit-talkable-action.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/auto-send-action.h"
#include "gui/widgets/chat-widget/bold-action.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/chat-widget/clear-chat-action.h"
#include "gui/widgets/chat-widget/insert-image-action.h"
#include "gui/widgets/chat-widget/italic-action.h"
#include "gui/widgets/chat-widget/more-actions-action.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/toolbar.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/open-chat-with/open-chat-with-service.h"
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

static void checkBlocking(Myself *myself, Action *action)
{
	BuddySet buddies = action->context()->buddies();

	if (!buddies.count() || buddies.contains(myself->buddy()))
	{
		action->setEnabled(false);
		return;
	}

	if (action->context()->buddies().isAnyTemporary())
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
}

ChatWidgetActions::~ChatWidgetActions()
{
}

void ChatWidgetActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void ChatWidgetActions::setAutoSendAction(AutoSendAction *autoSendAction)
{
	m_autoSendAction = autoSendAction;
}

void ChatWidgetActions::setBoldAction(BoldAction *boldAction)
{
    m_boldAction = boldAction;
}

void ChatWidgetActions::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
	m_chatConfigurationHolder = chatConfigurationHolder;
}

void ChatWidgetActions::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void ChatWidgetActions::setClearChatAction(ClearChatAction *clearChatAction)
{
	m_clearChatAction = clearChatAction;
}

void ChatWidgetActions::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ChatWidgetActions::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void ChatWidgetActions::setInsertImageAction(InsertImageAction *insertImageAction)
{
	m_insertImageAction = insertImageAction;
}

void ChatWidgetActions::setItalicAction(ItalicAction *italicAction)
{
	m_italicAction = italicAction;
}

void ChatWidgetActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void ChatWidgetActions::setMoreActionsAction(MoreActionsAction *moreActionsAction)
{
	m_moreActionsAction = moreActionsAction;
}

void ChatWidgetActions::setMyself(Myself *myself)
{
	m_myself = myself;
}

void ChatWidgetActions::setOpenChatWithService(OpenChatWithService *openChatWithService)
{
	m_openChatWithService = openChatWithService;
}

void ChatWidgetActions::init()
{
	m_actions->blockSignals();

	Underline = m_injectedFactory->makeInjected<ActionDescription>(nullptr,
		ActionDescription::TypeChat, "underlineAction",
		this, SLOT(underlineActionActivated(QAction *, bool)),
		KaduIcon("format-text-underline"), tr("Underline"), true,
		disableNoGadu
	);

	Send = m_injectedFactory->makeInjected<ActionDescription>(nullptr,
		ActionDescription::TypeChat, "sendAction",
		this, SLOT(sendActionActivated(QAction *, bool)),
		KaduIcon("go-next"), tr("&Send"), false,
		disableEmptyTextBoxOrNotConnected
	);
	connect(Send, SIGNAL(actionCreated(Action *)), this, SLOT(sendActionCreated(Action *)));

	BlockUser = m_injectedFactory->makeInjected<ActionDescription>(nullptr,
		ActionDescription::TypeUser, "blockUserAction",
		this, SLOT(blockUserActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/block-buddy"), tr("Block Buddy"), true,
		[this](Action *action){ return checkBlocking(m_myself, action); }
	);

	// The last ActionDescription of each type will send actionLoaded() signal.
	m_actions->unblockSignals();

	OpenChat = m_injectedFactory->makeInjected<ActionDescription>(nullptr,
		ActionDescription::TypeUser, "chatAction",
		this, SLOT(openChatActionActivated(QAction *, bool)),
		KaduIcon("internet-group-chat"), tr("&Chat"), false,
		disableNoChat
	);

	m_menuInventory
		->menu("buddy-list")
		->addAction(OpenChat, KaduMenu::SectionChat, 1000);

	OpenWith = m_injectedFactory->makeInjected<ActionDescription>(nullptr,
		ActionDescription::TypeGlobal, "openChatWithAction",
		this, SLOT(openChatWithActionActivated(QAction *, bool)),
		KaduIcon("internet-group-chat"), tr("Open Chat with...")
	);
	OpenWith->setShortcut("kadu_openchatwith", Qt::ApplicationShortcut);
/*
	ColorSelector = m_injectedFactory->makeInjected<ActionDescription>(nullptr,
		ActionDescription::TypeChat, "colorAction",
		this, SLOT(colorSelectorActionActivated(QAction *, bool)),
		KaduIcon("kadu_icons/change-color"), tr("Change Color")
	);*/

	EditTalkable = m_injectedFactory->makeInjected<EditTalkableAction>(this);
	m_actions->insert(EditTalkable);

	LeaveChat = m_injectedFactory->makeInjected<LeaveChatAction>(this);
	m_actions->insert(LeaveChat);
}

void ChatWidgetActions::done()
{
	delete Underline;
	delete Send;
	delete BlockUser;
	delete OpenChat;
	delete OpenWith;
	delete EditTalkable;
	delete LeaveChat;
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

	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	m_chatWidgetManager->openChat(action->context()->chat(), OpenChatActivation::Activate);

	kdebugf2();
}

void ChatWidgetActions::openChatWithActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	m_openChatWithService->show();

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

ActionDescription * ChatWidgetActions::bold() const
{
	return m_boldAction;
}

ActionDescription * ChatWidgetActions::italic() const
{
	return m_italicAction;
}

#include "moc_chat-widget-actions.cpp"
