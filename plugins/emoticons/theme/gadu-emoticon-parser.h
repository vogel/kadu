/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_EMOTICON_PARSER_H
#define GADU_EMOTICON_PARSER_H

#include <QtCore/QString>
#include <QtCore/QVector>

#include "emoticon.h"

class GaduEmoticonParser
{
	QString ThemePath;

	QString EmoticonLine;
	int Index;

	Emoticon Result;
	QVector<Emoticon> Aliases;

	QChar peek();
	QChar get();
	void eat();
	void parse();
	QStringList parseAliases();
	QString parseQuoted();
	QString getToIndex(int endIndexExclusive);

public:
	GaduEmoticonParser(const QString &themePath, const QString &emoticonLine);

	Emoticon emoticon() const;
	QVector<Emoticon> aliases() const;

};

#endif // GADU_EMOTICON_PARSER_H
