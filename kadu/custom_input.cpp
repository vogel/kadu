/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_dialog.h"
#include "custom_input.h"
#include "debug.h"

CustomInput::CustomInput(QWidget* parent, const char* name)
	: QMultiLineEdit(parent, name)
{
	kdebugf();
	QStyleSheet *style=styleSheet();
	style->item("p")->setMargin(QStyleSheetItem::MarginVertical, 0);
	setStyleSheet(style);
	kdebugf2();
}

void CustomInput::keyPressEvent(QKeyEvent* e)
{
//	kdebugf();
	emit keyPressed(e, this);
	if (autosend_enabled && ((HotKey::shortCut(e,"ShortCuts", "chat_newline")) || e->key()==Key_Enter)&& !(e->state() & ShiftButton))
	{
		kdebugmf(KDEBUG_INFO, "emit sendMessage()\n");
		emit sendMessage();
	}
	else
	{
		if (e->key() == Key_Minus)
		{
			insert("-");
			return;
		}
		if (e->text() == "*")
		{
			insert("*");
			return;
		}
		if (HotKey::shortCut(e,"ShortCuts", "chat_bold"))
		{
			emit specialKeyPressed(CustomInput::KEY_BOLD);
			return;
		}
		else if (HotKey::shortCut(e,"ShortCuts", "chat_italic"))
		{
			emit specialKeyPressed(CustomInput::KEY_ITALIC);
			return;
		}
		else if (HotKey::shortCut(e,"ShortCuts", "chat_underline"))
		{
			emit specialKeyPressed(CustomInput::KEY_UNDERLINE);
			return;
		}
		QMultiLineEdit::keyPressEvent(e);
	}
	// przekazanie event'a do qwidget
	// aby obsluzyc skroty klawiszowe (definiowane sa dla okna chat)
	QWidget::keyPressEvent(e);
//	kdebugf2();
}

void CustomInput::keyReleaseEvent(QKeyEvent* e)
{
	emit keyReleased(e, this);
	QWidget::keyReleaseEvent(e);
}

void CustomInput::setAutosend(bool on)
{
	autosend_enabled = on;
}

void CustomInput::paste()
{
	pasteSubType("plain");
}

