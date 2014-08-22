/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class GaduEmoticonParser
 * @short This class parses one line from Gadu Gadu emoticon theme file: emots.txt.
 * @author Rafał 'Vogel' Malinowski
 *
 * Use this class to parse one line  from Gadu Gadu emoticon theme file. If this line is valid, emoticon and all of
 * its aliases will be returned by this class.
 */
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
	/**
	 * @short Parse one line from Gadu Gadu emoticon theme file.
	 * @author Rafał 'Vogel' Malinowski
	 * @param themePath path of theme, used to construct absolute file paths of emoticons
	 * @param emoticonLine one line in Gadu Gadu emoticon format
	 */
	GaduEmoticonParser(const QString &themePath, const QString &emoticonLine);

	/**
	 * @short Return parsed emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @return parsed emoticon
	 *
	 * Returned emoticon can be null if line was in invalid format.
	 */
	Emoticon emoticon() const;

	/**
	 * @short Return parsed aliases.
	 * @author Rafał 'Vogel' Malinowski
	 * @return parsed aliases
	 *
	 * Returned aliases can be empty if line was in invalid format.
	 */
	QVector<Emoticon> aliases() const;

};

/**
 * @}
 */

#endif // GADU_EMOTICON_PARSER_H
