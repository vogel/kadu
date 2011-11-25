/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QMap>

#include "parser/parser-token-type.h"
#include "talkable/talkable.h"

#include "exports.h"

template<typename T> class QStack;

class ParserToken;

class KADUAPI Parser
{
	typedef QString (*ObjectTagCallback)(const QObject * const);
	typedef QString (*TalkableTagCallback)(Talkable);

	static QMap<QString, TalkableTagCallback> RegisteredTalkableTags;
	static QMap<QString, ObjectTagCallback> RegisteredObjectTags;

	static QString executeCmd(const QString &cmd);

	static bool isActionParserTokenAtTop(const QStack<ParserToken> &parseStack, const QVector<ParserTokenType> &acceptedTokens);
	static ParserToken parsePercentSyntax(const QString &s, int &idx, const Talkable &talkable, bool escape);

	template<typename ContainerClass>
	static QString joinParserTokens(const ContainerClass &parseStack);

public:
	static QMap<QString, QString> GlobalVariables;

	static QString parse(const QString &s, const QObject * const object, bool escape = true)
	{
		return parse(s, Talkable(), object, escape);
	}
	static QString parse(const QString &s, Talkable talkable, bool escape = true)
	{
		return parse(s, talkable, 0, escape);
	}
	static QString parse(const QString &s, Talkable talkable, const QObject * const object, bool escape = true);

	static bool registerTag(const QString &name, TalkableTagCallback);
	static bool unregisterTag(const QString &name);

	static bool registerObjectTag(const QString &name, ObjectTagCallback);
	static bool unregisterObjectTag(const QString &name);

};

#endif // PARSER_H
