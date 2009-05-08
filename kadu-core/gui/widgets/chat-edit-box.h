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

#include "action.h"
#include "configuration_aware_object.h"
#include "kadu_main_window.h"

#include "configuration_aware_object.h"

#include "exports.h"

class CustomInput;

class KADUAPI ChatEditBox : public KaduMainWindow, ConfigurationAwareObject
{
	Q_OBJECT

	CustomInput *InputBox;
	QColor CurrentColor;

	void setColorFromCurrentText(bool force);

private slots:
	void colorSelectorActionCreated(KaduAction *action);
	void cursorPositionChanged();

protected:
	virtual void configurationUpdated();

public:
	static void createDefaultToolbars(QDomElement parentConfig);
	static void addAction(const QString &actionName, bool showLabel = false);

	ChatEditBox(QWidget *parent);
	virtual ~ChatEditBox();

	// TODO: remove?
	CustomInput * inputBox();

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual ContactsListWidget * contactsListWidget();
	virtual ContactSet contacts();
	ChatWidget * chatWidget();

	void openEmoticonSelector(const QWidget *activatingWidget);
	void openColorSelector(const QWidget *activatingWidget);

	void setAutoSend(bool autoSend);

public slots:
	void addEmoticon(const QString &emoticon);
	void changeColor(const QColor &newColor);

};

#endif // CHAT_EDIT_BOX_H
