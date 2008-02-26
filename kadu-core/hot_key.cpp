/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <q3accel.h>
#include <qevent.h>
//Added by qt3to4:
#include <QKeyEvent>

#include "config_file.h"

#include "hot_key.h"

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
	if ((e->state() & Qt::ControlButton) || (e->key() == Qt::Key_Control))
		result = "Ctrl+";

	if ((e->state() & Qt::MetaButton) || (e->key() == Qt::Key_Meta))
		result += "Shift+Alt+";
	else
	{
		if ((e->state() & Qt::ShiftButton) || (e->key() == Qt::Key_Shift))
			result+= "Shift+";
		if ((e->state() & Qt::AltButton) || (e->key() == Qt::Key_Alt))
			result += "Alt+";
	}

	if (!((e->key() == Qt::Key_Control)
		||(e->key() == Qt::Key_Shift)
		||(e->key() == Qt::Key_Alt)
		||(e->key() == Qt::Key_Meta)))
			result += Q3Accel::keyToString(QKeySequence(e->key()));

	return result;
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

void HotKeyEdit::setShortCut(const QString &shortcut)
{
	QKeySequence str(shortcut);
	if (str == QKeySequence())
		setText(QString::null);
	else
		setText(shortcut);
}
