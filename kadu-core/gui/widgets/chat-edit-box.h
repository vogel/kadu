/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_EDIT_BOX_H
#define CHAT_EDIT_BOX_H

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-window.h"

#include "exports.h"

class Action;
class BaseActionContext;
class ChatWidget;
class CustomInput;

class KADUAPI ChatEditBox : public MainWindow, public ConfigurationAwareObject
{
	Q_OBJECT

	Chat CurrentChat;
	CustomInput *InputBox;
	QColor CurrentColor;

	BaseActionContext *Context;

	void setColorFromCurrentText(bool force);

private slots:
	void configurationUpdated();

	void updateContext();

	void fontChanged(QFont font);
	void colorSelectorActionCreated(Action *action);
	void cursorPositionChanged();

public:
	static void createDefaultToolbars(QDomElement parentConfig);

	ChatEditBox(const Chat &chat, QWidget *parent = 0);
	virtual ~ChatEditBox();

	// TODO: remove?
	CustomInput * inputBox();

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual TalkableProxyModel * talkableProxyModel();

	ChatWidget * chatWidget();

	void openColorSelector(const QWidget *activatingWidget);
	void openInsertImageDialog();

	void setAutoSend(bool autoSend);

public slots:
	void changeColor(const QColor &newColor);
	void insertPlainText(const QString &plainText);

signals:
	void keyPressed(QKeyEvent *e, CustomInput *sender, bool &handled);

};

#endif // CHAT_EDIT_BOX_H
