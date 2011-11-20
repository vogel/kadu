/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef SYNTAX_LIST_H
#define SYNTAX_LIST_H

#include <QtCore/QMap>
#include <QtCore/QObject>

struct SyntaxInfo
{
	bool global;
};

class SyntaxList : public QObject, public QMap<QString, SyntaxInfo>
{
	Q_OBJECT

	QString category;

public:
	SyntaxList(const QString &category);
	virtual ~SyntaxList() {}

	static QString readSyntax(const QString &category, const QString &name, const QString &defaultSyntax);

	void reload();

	bool updateSyntax(const QString &name, const QString &syntax);
	QString readSyntax(const QString &name);
	bool deleteSyntax(const QString &name);

	bool isGlobal(const QString &name);

signals:
	void updated();

};

#endif // SYNTAX_LIST_H
