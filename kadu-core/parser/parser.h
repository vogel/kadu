/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#ifndef PARSER_H
#define PARSER_H

#include <functional>
#include <QtCore/QMap>

#include "parser/parser-token-type.h"
#include "talkable/talkable.h"

#include "exports.h"

template<typename T> class QStack;

class ParserData;
class ParserToken;

enum class ParserEscape
{
	NoEscape,
	HtmlEscape
};

class KADUAPI Parser
{
	using ObjectTagCallback = std::function<QString(const ParserData * const)>;
	using TalkableTagCallback = std::function<QString(Talkable)>;

	static QMap<QString, TalkableTagCallback> RegisteredTalkableTags;
	static QMap<QString, ObjectTagCallback> RegisteredObjectTags;

	static QString executeCmd(const QString &cmd);

	static bool isActionParserTokenAtTop(const QStack<ParserToken> &parseStack, const QVector<ParserTokenType> &acceptedTokens);
	static ParserToken parsePercentSyntax(const QString &s, int &idx, const Talkable &talkable, ParserEscape escape);

	template<typename ContainerClass>
	static QString joinParserTokens(const ContainerClass &parseStack);

public:
	static QMap<QString, QString> GlobalVariables;

	static QString escape(const QString &string);

	static QString parse(const QString &s, const ParserData * const parserData, ParserEscape escape)
	{
		return parse(s, Talkable(), parserData, escape);
	}
	static QString parse(const QString &s, Talkable talkable, ParserEscape escape)
	{
		return parse(s, talkable, 0, escape);
	}
	static QString parse(const QString &s, Talkable talkable, const ParserData * const parserData, ParserEscape escape);

	static bool registerTag(const QString &name, TalkableTagCallback);
	static bool unregisterTag(const QString &name);

	static bool registerObjectTag(const QString &name, ObjectTagCallback);
	static bool unregisterObjectTag(const QString &name);

};

#endif // PARSER_H
