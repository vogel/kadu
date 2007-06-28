/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "custom_input.h"
#include "debug.h"
#include "hot_key.h"

CustomInput::CustomInput(QWidget* parent, const char* name)
	: QMultiLineEdit(parent, name), autosend_enabled(true)
{
	kdebugf();
	QStyleSheet *style=styleSheet();
	style->item("p")->setMargin(QStyleSheetItem::MarginVertical, 0);
	setStyleSheet(style);
	kdebugf2();
}

void CustomInput::keyPressEvent(QKeyEvent* e)
{
	kdebugf();
	bool handled = false;
	emit keyPressed(e, this, handled);
	if (handled)
	{
		e->accept();
		kdebugf2();
		return;
	}

	if (autosend_enabled && ((HotKey::shortCut(e,"ShortCuts", "chat_newline")) || e->key()==Key_Enter)&& !(e->state() & ShiftButton))
	{
		kdebugmf(KDEBUG_INFO, "emit sendMessage()\n");
		emit sendMessage();
		e->accept();
		kdebugf2();
		return;
	}
	else
	{
		if (e->key() == Key_Minus)
		{
			insert("-");
			e->accept();
			kdebugf2();
			return;
		}
		if (e->text() == "*")
		{
			insert("*");
			e->accept();
			kdebugf2();
			return;
		}
 		if (HotKey::shortCut(e,"ShortCuts", "chat_bold"))
 		{
 			emit specialKeyPressed(CustomInput::KEY_BOLD);
 			e->accept();
 			kdebugf2();
 			return;
 		}
 		else if (HotKey::shortCut(e,"ShortCuts", "chat_italic"))
 		{
 			emit specialKeyPressed(CustomInput::KEY_ITALIC);
 			e->accept();
 			kdebugf2();
 			return;
 		}
 		else if (HotKey::shortCut(e,"ShortCuts", "chat_underline"))
 		{
 			emit specialKeyPressed(CustomInput::KEY_UNDERLINE);
 			e->accept();
 			kdebugf2();
 			return;
 		}
	}
	QMultiLineEdit::keyPressEvent(e);
	kdebugf2();
}

void CustomInput::keyReleaseEvent(QKeyEvent* e)
{
	bool handled = false;
	emit keyReleased(e, this, handled);
	if (handled)
	{
		e->accept();
		return;
	}

	QMultiLineEdit::keyReleaseEvent(e);
}

void CustomInput::setAutosend(bool on)
{
	autosend_enabled = on;
}

void CustomInput::paste()
{
	pasteSubType("plain");
}

