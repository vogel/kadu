/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
	ActionDescription *Whois;
	ActionDescription *BlockUser;
	ActionDescription *OpenChat;
	ActionDescription *OpenWith;
	ActionDescription *InsertEmoticon;
// 	ActionDescription *ColorSelector;

	void autoSendActionCheck();
	void insertEmoticonsActionCheck();
	void updateBlockingActions(Buddy buddy);

private slots:
	void autoSendActionCreated(Action *action);
	void clearChatActionCreated(Action *action);
	void sendActionCreated(Action *action);
	void insertEmoticonActionCreated(Action *action);

	void moreActionsActionActivated(QAction *sender, bool toggled);
	void autoSendActionActivated(QAction *sender, bool toggled);
	void clearActionActivated(QAction *sender, bool toggled);
	void insertImageActionActivated(QAction *sender, bool toggled);
	void boldActionActivated(QAction *sender, bool toggled);
	void italicActionActivated(QAction *sender, bool toggled);
	void underlineActionActivated(QAction *sender, bool toggled);
	void sendActionActivated(QAction *sender, bool toggled);
	void whoisActionActivated(QAction *sender, bool toggled);
	void blockUserActionActivated(QAction *sender, bool toggled);
	void openChatActionActivated(QAction *sender, bool toggled);
	void openChatWithActionActivated(QAction *sender, bool toggled);
	void colorSelectorActionActivated(QAction *sender, bool toogled);
	void insertEmoticonActionActivated(QAction *sender, bool toggled);

protected:
	virtual void configurationUpdated();

public:
	explicit ChatWidgetActions(QObject *parent);
	virtual ~ChatWidgetActions();

	ActionDescription * bold() { return Bold; }
	ActionDescription * italic() { return Italic; }
	ActionDescription * underline() { return Underline; }
	ActionDescription * send() { return Send; }
	ActionDescription * blockUser() { return BlockUser; }
	ActionDescription * openChatWith() { return OpenWith; }
// 	ActionDescription * colorSelector() { return ColorSelector; }
	ActionDescription * insertEmoticon() { return InsertEmoticon; }

};

#endif // CHAT_WIDGET_ACTIONS
