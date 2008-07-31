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

#include <action.h>
#include <kadu_main_window.h>

#include "configuration_aware_object.h"

#include "exports.h"

class CustomInput;

class KADUAPI ChatEditBox : public KaduMainWindow
{
	Q_OBJECT

	CustomInput *InputBox;

public:
	ChatEditBox(QWidget *parent);
	virtual ~ChatEditBox();

	static void createDefaultToolbars(QDomElement parentConfig);
	static void addAction(const QString &actionName, bool showLabel = false);

	// TODO: remove?
	CustomInput * inputBox();

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual UserBox * userBox();
	virtual UserListElements userListElements();
	ChatWidget * chatWidget();

};

#endif // CHAT_EDIT_BOX_H
