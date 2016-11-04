/*
 * %kadu copyright begin%
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

#pragma once

#include "configuration/configuration-aware-object.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QAction;

class ActionDescription;
class Actions;
class Action;
class AutoSendAction;
class BlockUserAction;
class BoldAction;
class Buddy;
class ChatConfigurationHolder;
class ChatWidgetManager;
class ClearChatAction;
class Configuration;
class EditTalkableAction;
class InjectedFactory;
class InsertImageAction;
class ItalicAction;
class LeaveChatAction;
class MenuInventory;
class MoreActionsAction;
class Myself;
class OpenChatAction;
class OpenChatWithAction;
class OpenChatWithService;
class UnderlineAction;
class SendAction;

class ChatWidgetActions : public QObject
{
	Q_OBJECT

	QPointer<Actions> m_actions;
	QPointer<AutoSendAction> m_autoSendAction;
	QPointer<BlockUserAction> m_blockUserAction;
	QPointer<BoldAction> m_boldAction;
	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ClearChatAction> m_clearChatAction;
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<InsertImageAction> m_insertImageAction;
	QPointer<ItalicAction> m_italicAction;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<MoreActionsAction> m_moreActionsAction;
	QPointer<Myself> m_myself;
	QPointer<OpenChatAction> m_openChatAction;
	QPointer<OpenChatWithAction> m_openChatWithAction;
	QPointer<SendAction> m_sendAction;
	QPointer<UnderlineAction> m_underlineAction;

	EditTalkableAction *EditTalkable;
	LeaveChatAction *LeaveChat;

	void autoSendActionCheck();
	void updateBlockingActions(Buddy buddy);

private slots:
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setAutoSendAction(AutoSendAction *autoSendAction);
	INJEQT_SET void setBlockUserAction(BlockUserAction *blockUserAction);
	INJEQT_SET void setBoldAction(BoldAction *boldAction);
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setClearChatAction(ClearChatAction *clearChatAction);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setInsertImageAction(InsertImageAction *insertImageAction);
	INJEQT_SET void setItalicAction(ItalicAction *italicAction);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setMoreActionsAction(MoreActionsAction *moreActionsAction);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_SET void setOpenChatAction(OpenChatAction *openChatAction);
	INJEQT_SET void setOpenChatWithAction(OpenChatWithAction *openChatWithAction);
	INJEQT_SET void setSendAction(SendAction *sendAction);
	INJEQT_SET void setUnderlineAction(UnderlineAction *underlineAction);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void colorSelectorActionActivated(QAction *sender, bool toogled);

public:
	Q_INVOKABLE explicit ChatWidgetActions(QObject *parent = nullptr);
	virtual ~ChatWidgetActions();

	ActionDescription * bold() const;
	ActionDescription * italic() const;
	ActionDescription * underline() const;
	ActionDescription * send() const;
	ActionDescription * blockUser() const;
	ActionDescription * openChatWith() const;
// 	ActionDescription * colorSelector() { return ColorSelector; }

};
