/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_EDIT_BOX_H
#define CHAT_EDIT_BOX_H

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-window.h"

#include "exports.h"

class Action;
class ChatWidget;
class CustomInput;

class KADUAPI ChatEditBox : public MainWindow, ConfigurationAwareObject
{
	Q_OBJECT

	Chat CurrentChat;
	CustomInput *InputBox;
	QColor CurrentColor;

	void setColorFromCurrentText(bool force);

private slots:
	void fontChanged(QFont font);
	void colorSelectorActionCreated(Action *action);
	void cursorPositionChanged();

protected:
	virtual void configurationUpdated();

public:
	static void createDefaultToolbars(QDomElement parentConfig);
	static void addAction(const QString &actionName, Qt::ToolButtonStyle style = Qt::ToolButtonIconOnly);

	ChatEditBox(Chat chat, QWidget *parent = 0);
	virtual ~ChatEditBox();

	// TODO: remove?
	CustomInput * inputBox();

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual BuddiesListView * contactsListView();
	virtual ContactSet contacts();
	virtual BuddySet buddies();
	virtual Chat chat() { return CurrentChat; }
	ChatWidget * chatWidget();

	void openEmoticonSelector(const QWidget *activatingWidget);
	void openColorSelector(const QWidget *activatingWidget);
	void openInsertImageDialog();

	void setAutoSend(bool autoSend);

public slots:
	void addEmoticon(const QString &emoticon);
	void changeColor(const QColor &newColor);

signals:
	void keyPressed(QKeyEvent *e, CustomInput *sender, bool &handled);

};

#endif // CHAT_EDIT_BOX_H
