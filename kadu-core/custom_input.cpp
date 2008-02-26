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
//Added by qt3to4:
#include <QKeyEvent>

CustomInput::CustomInput(QWidget* parent, const char* name)
	: Q3MultiLineEdit(parent, name), autosend_enabled(true)
{
	kdebugf();
	Q3StyleSheet *style=styleSheet();
	style->item("p")->setMargin(Q3StyleSheetItem::MarginVertical, 0);
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

	if (autosend_enabled && ((HotKey::shortCut(e, "ShortCuts", "chat_newline")) || e->key() == Qt::Key_Enter) && !(e->state() & Qt::ShiftButton))
	{
		kdebugmf(KDEBUG_INFO, "emit sendMessage()\n");
		emit sendMessage();
		e->accept();
		kdebugf2();
		return;
	}
	else
	{
		if (e->key() == Qt::Key_Minus)
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
 		if (HotKey::shortCut(e, "ShortCuts", "chat_bold"))
 		{
 			emit specialKeyPressed(CustomInput::KEY_BOLD);
 			e->accept();
 			kdebugf2();
 			return;
 		}
 		if (HotKey::shortCut(e, "ShortCuts", "chat_italic"))
 		{
 			emit specialKeyPressed(CustomInput::KEY_ITALIC);
 			e->accept();
 			kdebugf2();
 			return;
 		}
 		if (HotKey::shortCut(e, "ShortCuts", "chat_underline"))
 		{
 			emit specialKeyPressed(CustomInput::KEY_UNDERLINE);
 			e->accept();
 			kdebugf2();
 			return;
 		}
		if (e->key() == Qt::Key_A && (e->state() & Qt::ControlButton))
		{
			selectAll();	
			e->accept();
 			kdebugf2();
 			return;
		}
	}
	Q3MultiLineEdit::keyPressEvent(e);
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

	Q3MultiLineEdit::keyReleaseEvent(e);
}

void CustomInput::setAutosend(bool on)
{
	autosend_enabled = on;
}

void CustomInput::paste()
{
	pasteSubType("plain");
}

