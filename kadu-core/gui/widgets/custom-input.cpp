/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>

#include "gui/hot-key.h"
#include "debug.h"

#include "custom-input.h"

CustomInput::CustomInput(QWidget *parent) :
		QTextEdit(parent), CopyPossible(false), autosend_enabled(true)
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

	/* Ctrl+Return and Ctrl+Enter have a special meaning:
	 * 1) autosend_enabled -> new line is entered
	 * 2) message is sent
	 */
	bool isCtrlEnter = (e->key() == Qt::Key_Return && e->modifiers() == Qt::ControlModifier)
			|| (e->key() == Qt::Key_Enter && e->modifiers() == (Qt::KeypadModifier | Qt::ControlModifier));

	if ((autosend_enabled
			&& ((e->key() == Qt::Key_Return && e->modifiers() == Qt::NoModifier)
				|| (e->key() == Qt::Key_Enter && e->modifiers() == Qt::KeypadModifier)))
		|| (!autosend_enabled && isCtrlEnter))
	{
		kdebugmf(KDEBUG_INFO, "emit sendMessage()\n");
		emit sendMessage();
		e->accept();
		kdebugf2();
		return;
	}
	else if (isCtrlEnter)
	{
		// now surely autosend_enabled == true, so we can emulate Shift+Return to get a new line
		QKeyEvent emulateNewLineEvent(QEvent::KeyPress, Qt::Key_Return, Qt::ShiftModifier, "\n", false, 2);
		QTextEdit::keyPressEvent(&emulateNewLineEvent);

		e->accept();
		kdebugf2();
		return;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "chat_bold"))
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
	else if (HotKey::shortCut(e, "ShortCuts", "chat_italic"))
	{
		setFontItalic(!fontItalic());

		emit fontChanged(currentFont());

		e->accept();
		kdebugf2();
		return;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "chat_underline"))
	{
		setFontUnderline(!fontUnderline());

		emit fontChanged(currentFont());

		e->accept();
		kdebugf2();
		return;
	}
	else if (e->matches(QKeySequence::SelectAll))
	{
		selectAll();
		e->accept();
		kdebugf2();
		return;
	}
	else if (CopyPossible && e->matches(QKeySequence::Copy))
	{
		copy();
		e->accept();
		kdebugf2();
		return;
	}

	bool handled = false;
	emit keyPressed(e, this, handled);
	if (handled)
	{
		e->accept();
		kdebugf2();
		return;
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
	QMenu menu(this);

	QAction *undo = new QAction(tr("Undo"), &menu);
	undo->setShortcut(QKeySequence::Undo);
	connect(undo, SIGNAL(triggered()), this, SLOT(undo()));
	menu.addAction(undo);

	QAction *redo = new QAction(tr("Redo"), &menu);
	redo->setShortcut(QKeySequence::Redo);
	connect(redo, SIGNAL(triggered()), this, SLOT(redo()));
	menu.addAction(redo);

	menu.addSeparator();

	QAction *cut = new QAction(tr("Cut"), &menu);
	cut->setShortcut(QKeySequence::Cut);
	connect(cut, SIGNAL(triggered()), this, SLOT(cut()));
	menu.addAction(cut);

	QAction *copy = new QAction(tr("Copy"), &menu);
	copy->setShortcut(QKeySequence::Copy);
	connect(copy, SIGNAL(triggered()), this, SLOT(copy()));
	menu.addAction(copy);

	QAction *paste = new QAction(tr("Paste"), &menu);
	paste->setShortcut(QKeySequence::Paste);
	connect(paste, SIGNAL(triggered()), this, SLOT(paste()));
	menu.addAction(paste);

	QAction *clear = new QAction(tr("Clear"), &menu);
	connect(clear, SIGNAL(triggered()), this, SLOT(clear()));
	menu.addAction(clear);

	menu.addSeparator();

	QAction *all = new QAction(tr("Select All"), &menu);
	all->setShortcut(QKeySequence::SelectAll);
	connect(all, SIGNAL(triggered()), this, SLOT(selectAll()));
	menu.addAction(all);

	menu.exec(e->globalPos());
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
