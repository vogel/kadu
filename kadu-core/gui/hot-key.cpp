/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QKeyEvent>

#include "configuration/configuration-file.h"

#include "hot-key.h"

QKeySequence HotKey::shortCutFromFile(const QString &groupname, const QString &name)
{
	return QKeySequence(config_file.readEntry(groupname, name));
}

bool HotKey::shortCut(QKeyEvent *e, const QString &groupname, const QString &name)
{
	return config_file.readEntry(groupname, name) == keyEventToString(e);
}

QString HotKey::keyEventToString(QKeyEvent *e)
{
	QString result;
	if ((e->modifiers() & Qt::ControlModifier) || (e->key() == Qt::Key_Control))
		result = "Ctrl+";

	if ((e->modifiers() & Qt::MetaModifier) || (e->key() == Qt::Key_Meta))
		result += "Shift+Alt+";
	else
	{
		if ((e->modifiers() & Qt::ShiftModifier) || (e->key() == Qt::Key_Shift))
			result+= "Shift+";
		if ((e->modifiers() & Qt::AltModifier) || (e->key() == Qt::Key_Alt))
			result += "Alt+";
	}

	if (!((e->key() == Qt::Key_Control) ||
		(e->key() == Qt::Key_Shift) ||
		(e->key() == Qt::Key_Alt) ||
		(e->key() == Qt::Key_Meta)))
		result += QKeySequence(e->key()).toString();

	return result;
}

HotKeyEdit::HotKeyEdit(QWidget *parent)
	: QLineEdit(parent)
{
}

QString HotKeyEdit::shortCutString() const
{
	return text();
}

QKeySequence HotKeyEdit::shortCut() const {
	return QKeySequence(text());
}

void HotKeyEdit::setShortCut(const QString &shortcut)
{
	QKeySequence str(shortcut);
	if (str == QKeySequence())
		setText(QString::null);
	else
		setText(shortcut);
}

void HotKeyEdit::setShortCut(const QKeySequence &shortcut) {
	setText(shortcut);
}

void HotKeyEdit::keyPressEvent(QKeyEvent *e)
{
	setText(HotKey::keyEventToString(e));
}

void HotKeyEdit::keyReleaseEvent(QKeyEvent *)
{
	// sprawdzenie czy ostatnim znakiem jest "+"
	// jesli tak to nie ma takiego skrotu klawiszowego
	if (text().at(text().length() - 1) == '+')
		setText(QString::null);
}
