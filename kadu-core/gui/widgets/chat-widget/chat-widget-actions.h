/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_WIDGET_ACTIONS
#define CHAT_WIDGET_ACTIONS

#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"

class QAction;

class Action;
class ActionDescription;
class Buddy;
class EditTalkableAction;
class LeaveChatAction;

class ChatWidgetActions : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

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
	explicit ChatWidgetActions(QObject *parent);
	virtual ~ChatWidgetActions();

	ActionDescription * bold() const { return Bold; }
	ActionDescription * italic() const { return Italic; }
	ActionDescription * underline() const { return Underline; }
	ActionDescription * send() const { return Send; }
	ActionDescription * blockUser() const { return BlockUser; }
	ActionDescription * openChatWith() const { return OpenWith; }
// 	ActionDescription * colorSelector() { return ColorSelector; }

};

#endif // CHAT_WIDGET_ACTIONS
