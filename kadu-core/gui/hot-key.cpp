/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QKeyEvent>

#include "configuration/configuration-file.h"

#include "hot-key.h"

QKeySequence HotKey::shortCutFromFile(const QString &groupname, const QString &name)
{
	return QKeySequence(config_file.readEntry(groupname, name));
}

bool HotKey::shortCut(QKeyEvent *e, const QString &groupname, const QString &name)
{
	QString config = config_file.readEntry(groupname, name);
	return !config.isEmpty() && config == keyEventToString(e);
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
	: LineEditWithClearButton(parent)
{
}

QString HotKeyEdit::shortCutString() const
{
	return text();
}

QKeySequence HotKeyEdit::shortCut() const
{
	return QKeySequence(text());
}

void HotKeyEdit::setShortCut(const QString &shortcut)
{
	QKeySequence str(shortcut);
	if (str.isEmpty())
		clear();
	else
		setText(shortcut);
}

void HotKeyEdit::setShortCut(const QKeySequence &shortcut)
{
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
	if (text().isEmpty())
		return;
	if (text().at(text().length() - 1) == '+')
		clear();
}

#include "moc_hot-key.cpp"
