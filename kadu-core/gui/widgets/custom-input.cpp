/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>

#include "gui/hot-key.h"
#include "debug.h"

#include "custom-input.h"

CustomInput::CustomInput(QWidget *parent)
	: QTextEdit(parent), autosend_enabled(true), CopyPossible(false)
{
	kdebugf();

	setAcceptRichText(false);

	connect(this, SIGNAL(copyAvailable(bool)), this, SLOT(setCopyPossible(bool)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChangedSlot()));

	kdebugf2();
}

void CustomInput::keyPressEvent(QKeyEvent *e)
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

	if (autosend_enabled && ((HotKey::shortCut(e, "ShortCuts", "chat_newline"))
		|| e->key() == Qt::Key_Enter) && !(e->modifiers() & Qt::ShiftModifier))
	{
		kdebugmf(KDEBUG_INFO, "emit sendMessage()\n");
		emit sendMessage();
		e->accept();
		kdebugf2();
		return;
	}
	else
	{
 		if (HotKey::shortCut(e, "ShortCuts", "chat_bold"))
 		{
			if (QFont::Normal == fontWeight())
				setFontWeight(QFont::Bold);
			else
				setFontWeight(QFont::Normal);

			emit fontChanged(currentFont());

 			e->accept();
 			kdebugf2();
 			return;
 		}
 		if (HotKey::shortCut(e, "ShortCuts", "chat_italic"))
 		{
 			setFontItalic(!fontItalic());

			emit fontChanged(currentFont());

 			e->accept();
 			kdebugf2();
 			return;
 		}
 		if (HotKey::shortCut(e, "ShortCuts", "chat_underline"))
 		{
			setFontUnderline(!fontUnderline());
			
			emit fontChanged(currentFont());

 			e->accept();
 			kdebugf2();
 			return;
 		}
		if (e->key() == Qt::Key_A && (e->modifiers() & Qt::ControlModifier))
		{
			selectAll();	
			e->accept();
 			kdebugf2();
 			return;
		}
		if (!CopyPossible && e->key() == Qt::Key_C && e->modifiers() & Qt::ControlModifier)
		{
// 			emit specialKeyPressed(CustomInput::KEY_COPY);
 			e->accept();
 			kdebugf2();
 			return;	
		}
	}
	QTextEdit::keyPressEvent(e);
	kdebugf2();
}

void CustomInput::keyReleaseEvent(QKeyEvent *e)
{
	bool handled = false;
	emit keyReleased(e, this, handled);
	if (handled)
	{
		e->accept();
		return;
	}

	QTextEdit::keyReleaseEvent(e);
}

void CustomInput::contextMenuEvent(QContextMenuEvent *e)
{
	QMenu *menu = new QMenu();

	QAction *undo = new QAction(tr("Undo"), menu);
	undo->setShortcut(QKeySequence::Undo);
	connect(undo, SIGNAL(triggered()), this, SLOT(undo()));
	menu->addAction(undo);

	QAction *redo = new QAction(tr("Redo"), menu);
	redo->setShortcut(QKeySequence::Redo);
	connect(redo, SIGNAL(triggered()), this, SLOT(redo()));
	menu->addAction(redo);
	
	menu->addSeparator();

	QAction *cut = new QAction(tr("Cut"), menu);
	cut->setShortcut(QKeySequence::Cut);
	connect(cut, SIGNAL(triggered()), this, SLOT(cut()));
	menu->addAction(cut);

	QAction *copy = new QAction(tr("Copy"), menu);
	copy->setShortcut(QKeySequence::Copy);
	connect(copy, SIGNAL(triggered()), this, SLOT(copy()));
	menu->addAction(copy);

	QAction *paste = new QAction(tr("Paste"), menu);
	paste->setShortcut(QKeySequence::Paste);
	connect(paste, SIGNAL(triggered()), this, SLOT(paste()));
	menu->addAction(paste);

	QAction *clear = new QAction(tr("Clear"), menu);
	connect(clear, SIGNAL(triggered()), this, SLOT(clear()));
	menu->addAction(clear);

	menu->addSeparator();

	QAction *all = new QAction(tr("Select All"), menu);
	all->setShortcut(QKeySequence::SelectAll);
	connect(all, SIGNAL(triggered()), this, SLOT(selectAll()));
	menu->addAction(all);
	
	menu->exec(e->globalPos());
}

void CustomInput::setAutoSend(bool on)
{
	autosend_enabled = on;
}

void CustomInput::cursorPositionChangedSlot()
{
	emit fontChanged(currentFont());
}
	
void CustomInput::setCopyPossible(bool available)
{
	CopyPossible = available;
}
