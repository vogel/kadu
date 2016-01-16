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
class Buddy;
class ChatConfigurationHolder;
class ChatWidgetManager;
class Configuration;
class EditTalkableAction;
class InjectedFactory;
class LeaveChatAction;
class MenuInventory;
class Myself;

class ChatWidgetActions : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<Actions> m_actions;
	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<Myself> m_myself;

	ActionDescription *MoreActions;
	ActionDescription *AutoSend;
	ActionDescription *ClearChat;
	ActionDescription *InsertImage;
	ActionDescription *Bold;
	ActionDescription *Italic;
	ActionDescription *Underline;
	ActionDescription *Send;
	ActionDescription *BlockUser;
	ActionDescription *OpenChat;
	ActionDescription *OpenWith;
// 	ActionDescription *ColorSelector;

	EditTalkableAction *EditTalkable;
	LeaveChatAction *LeaveChat;

	void autoSendActionCheck();
	void updateBlockingActions(Buddy buddy);

private slots:
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void autoSendActionCreated(Action *action);
	void clearChatActionCreated(Action *action);
	void sendActionCreated(Action *action);

	void moreActionsActionActivated(QAction *sender, bool toggled);
	void autoSendActionActivated(QAction *sender, bool toggled);
	void clearActionActivated(QAction *sender, bool toggled);
	void insertImageActionActivated(QAction *sender, bool toggled);
	void boldActionActivated(QAction *sender, bool toggled);
	void italicActionActivated(QAction *sender, bool toggled);
	void underlineActionActivated(QAction *sender, bool toggled);
	void sendActionActivated(QAction *sender, bool toggled);
	void blockUserActionActivated(QAction *sender, bool toggled);
	void openChatActionActivated(QAction *sender, bool toggled);
	void openChatWithActionActivated(QAction *sender, bool toggled);
	void colorSelectorActionActivated(QAction *sender, bool toogled);

protected:
	virtual void configurationUpdated();

public:
	Q_INVOKABLE explicit ChatWidgetActions(QObject *parent = nullptr);
	virtual ~ChatWidgetActions();

	ActionDescription * bold() const { return Bold; }
	ActionDescription * italic() const { return Italic; }
	ActionDescription * underline() const { return Underline; }
	ActionDescription * send() const { return Send; }
	ActionDescription * blockUser() const { return BlockUser; }
	ActionDescription * openChatWith() const { return OpenWith; }
// 	ActionDescription * colorSelector() { return ColorSelector; }

};
