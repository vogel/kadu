/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QDomElement>

#include "config_file.h"
#include "custom_input.h"
#include "toolbar.h"

#include "chat_edit_box.h"

ChatEditBox::ChatEditBox(QWidget *parent)
	: KaduMainWindow(parent)
{
	InputBox = new CustomInput(this);
	InputBox->setWordWrapMode(QTextOption::WordWrap);
	InputBox->setTextFormat(Qt::RichText);

	setCentralWidget(InputBox);

	loadToolBarsFromConfig("chatTopDockArea", Qt::TopToolBarArea);
	loadToolBarsFromConfig("chatMiddleDockArea", Qt::TopToolBarArea);
	loadToolBarsFromConfig("chatBottomDockArea", Qt::BottomToolBarArea);
	loadToolBarsFromConfig("chatLeftDockArea", Qt::LeftToolBarArea);
	loadToolBarsFromConfig("chatRightDockArea", Qt::RightToolBarArea);

	loadToolBarsFromConfig("chat");
}

ChatEditBox::~ChatEditBox()
{
	writeToolBarsToConfig("chat");
}

CustomInput * ChatEditBox::inputBox()
{
	return InputBox;
}
