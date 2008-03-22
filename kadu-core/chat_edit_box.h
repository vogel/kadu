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

#include <kadu_main_window.h>

class CustomInput;

class ChatEditBox : public KaduMainWindow
{
	Q_OBJECT

	CustomInput *InputBox;

public:
	ChatEditBox(QWidget *parent);
	virtual ~ChatEditBox();

	// TODO: remove?
	CustomInput * inputBox();

};

#endif // CHAT_EDIT_BOX_H
