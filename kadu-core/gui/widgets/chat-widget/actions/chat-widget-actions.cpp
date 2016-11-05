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

#include "chat-widget-actions.h"

#include "core/injected-factory.h"
#include "gui/actions/actions.h"
#include "gui/actions/edit-talkable-action.h"
#include "gui/actions/chat/leave-chat-action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-widget/actions/auto-send-action.h"
#include "gui/widgets/chat-widget/actions/block-user-action.h"
#include "gui/widgets/chat-widget/actions/bold-action.h"
#include "gui/widgets/chat-widget/actions/clear-chat-action.h"
#include "gui/widgets/chat-widget/actions/insert-image-action.h"
#include "gui/widgets/chat-widget/actions/italic-action.h"
#include "gui/widgets/chat-widget/actions/more-actions-action.h"
#include "gui/widgets/chat-widget/actions/open-chat-action.h"
#include "gui/widgets/chat-widget/actions/open-chat-with-action.h"
#include "gui/widgets/chat-widget/actions/underline-action.h"
#include "gui/widgets/chat-widget/actions/send-action.h"

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

void ChatWidgetActions::setBlockUserAction(BlockUserAction *blockUserAction)
{
	m_blockUserAction = blockUserAction;
}

void ChatWidgetActions::setBoldAction(BoldAction *boldAction)
{
    m_boldAction = boldAction;
}

void ChatWidgetActions::setClearChatAction(ClearChatAction *clearChatAction)
{
	m_clearChatAction = clearChatAction;
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

void ChatWidgetActions::setOpenChatAction(OpenChatAction *openChatAction)
{
	m_openChatAction = openChatAction;
}

void ChatWidgetActions::setOpenChatWithAction(OpenChatWithAction *openChatWithAction)
{
	m_openChatWithAction = openChatWithAction;
}

void ChatWidgetActions::setSendAction(SendAction *sendAction)
{
	m_sendAction = sendAction;
}

void ChatWidgetActions::setUnderlineAction(UnderlineAction *underlineAction)
{
	m_underlineAction = underlineAction;
}

void ChatWidgetActions::init()
{
	m_menuInventory
		->menu("buddy-list")
		->addAction(m_openChatAction, KaduMenu::SectionChat, 1000);

	m_openChatWithAction->setShortcut("kadu_openchatwith", Qt::ApplicationShortcut);

	EditTalkable = m_injectedFactory->makeInjected<EditTalkableAction>(this);
	m_actions->insert(EditTalkable);

	LeaveChat = m_injectedFactory->makeInjected<LeaveChatAction>(this);
	m_actions->insert(LeaveChat);
}

void ChatWidgetActions::done()
{
	delete EditTalkable;
	delete LeaveChat;
}

ActionDescription * ChatWidgetActions::bold() const
{
	return m_boldAction;
}

ActionDescription * ChatWidgetActions::italic() const
{
	return m_italicAction;
}

ActionDescription * ChatWidgetActions::underline() const
{
	return m_underlineAction;
}

ActionDescription * ChatWidgetActions::blockUser() const
{
	return m_blockUserAction;
}

ActionDescription * ChatWidgetActions::openChatWith() const
{
	return m_openChatWithAction;
}

#include "moc_chat-widget-actions.cpp"
