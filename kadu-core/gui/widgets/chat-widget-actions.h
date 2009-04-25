/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_WIDGET_ACTIONS
#define CHAT_WIDGET_ACTIONS

#include <QtCore/QObject>

#include "configuration_aware_object.h"

class QAction;

class ActionDescription;
class KaduAction;

class ChatWidgetActions : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	ActionDescription *AutoSend;
	ActionDescription *ClearChat;
	ActionDescription *InsertImage;
	ActionDescription *Bold;
	ActionDescription *Italic;
	ActionDescription *Underline;
	ActionDescription *Send;
	ActionDescription *Whois;
	ActionDescription *IgnoreUser;
	ActionDescription *BlockUser;
	ActionDescription *OpenChat;
	ActionDescription *OpenChatWith;
	ActionDescription *InsertEmoticon;
	ActionDescription *ColorSelector;

	void autoSendActionCheck();

private slots:
	void autoSendActionCreated(KaduAction *action);
	void sendActionCreated(KaduAction *action);
	void insertEmoticonActionCreated(KaduAction *action);

	void autoSendActionActivated(QAction *sender, bool toggled);
	void clearActionActivated(QAction *sender, bool toggled);
	void insertImageActionActivated(QAction *sender, bool toggled);
	void boldActionActivated(QAction *sender, bool toggled);
	void italicActionActivated(QAction *sender, bool toggled);
	void underlineActionActivated(QAction *sender, bool toggled);
	void sendActionActivated(QAction *sender, bool toggled);
	void whoisActionActivated(QAction *sender, bool toggled);
	void ignoreUserActionActivated(QAction *sender, bool toggled);
	void blockUserActionActivated(QAction *sender, bool toggled);
	void openChatActionActivated(QAction *sender, bool toggled);
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
	ActionDescription * ignoreUser() { return IgnoreUser; }
	ActionDescription * blockUser() { return BlockUser; }
	ActionDescription * openChatWith() { return OpenChatWith; }
	ActionDescription * colorSelector() { return ColorSelector; }
	ActionDescription * insertEmoticon() { return InsertEmoticon; }

};

#endif // CHAT_WIDGET_ACTIONS
