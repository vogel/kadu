/*
 * %kadu copyright begin%
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef PARSER_TOKEN_H
#define PARSER_TOKEN_H

#include <QtCore/QString>

#include "parser/parser-token-type.h"

class ParserToken
{
	ParserTokenType Type;
	QString Content;
	bool IsContentEncoded;

public:
	ParserToken();

	ParserTokenType type() const { return Type; }
	void setType(ParserTokenType type);

	QString decodedContent() const;
	const QString & rawContent() const { return Content; } 
	void setContent(const QString &content);

	bool isEncoded() const { return IsContentEncoded; }

	void encodeContent(const QByteArray &exclude, const QByteArray &include);

};

#endif // PARSER_TOKEN_H
