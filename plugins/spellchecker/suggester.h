/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SUGGESTER_H
#define SUGGESTER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtGui/QTextCursor>
#include <QtWidgets/QAction>

class ActionDescription;

class Suggester : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Suggester);

	static Suggester *Instance;

	QStringList SuggestionWordList;
	QTextCursor CurrentTextSelection;
	QList<ActionDescription *> SuggestActions;

	Suggester();
	virtual ~Suggester();

	void buildSuggestList(const QString &word);
	void addWordListToMenu(const QTextCursor &textCursor);

protected:
	virtual bool eventFilter(QObject *object, QEvent *event);

public:
	static Suggester *instance();
	static void destroyInstance();

	void clearWordMenu();

public slots:
	void replaceWithSuggest(QAction *sender);

};

#endif // SUGGESTER_H
