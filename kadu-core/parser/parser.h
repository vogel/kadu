/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "parser/parser-token-type.h"
#include "talkable/talkable.h"
#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <functional>
#include <injeqt/injeqt.h>

template<typename T> class QStack;

class ChatDataExtractor;
class Configuration;
class ParserData;
class ParserToken;
class StatusContainerManager;
class StatusTypeManager;
class TalkableConverter;

enum class ParserEscape
{
	NoEscape,
	HtmlEscape
};

class KADUAPI Parser : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit Parser(QObject *parent = nullptr);
	virtual ~Parser();

	using ObjectTagCallback = std::function<QString(const ParserData * const)>;
	using TalkableTagCallback = std::function<QString(Talkable)>;

	QMap<QString, QString> GlobalVariables;

	QString escape(const QString &string);

	QString parse(const QString &s, const ParserData * const parserData, ParserEscape escape)
	{
		return parse(s, Talkable{}, parserData, escape);
	}
	QString parse(const QString &s, Talkable talkable, ParserEscape escape)
	{
		return parse(s, talkable, nullptr, escape);
	}
	QString parse(const QString &s, Talkable talkable, const ParserData * const parserData, ParserEscape escape);

	bool registerTag(const QString &name, TalkableTagCallback);
	bool unregisterTag(const QString &name);

	bool registerObjectTag(const QString &name, ObjectTagCallback);
	bool unregisterObjectTag(const QString &name);

private:
	QPointer<ChatDataExtractor> m_chatDataExtractor;
	QPointer<Configuration> m_configuration;
	QPointer<StatusContainerManager> m_statusContainerManager;
	QPointer<StatusTypeManager> m_statusTypeManager;
	QPointer<TalkableConverter> m_talkableConverter;

	QMap<QString, TalkableTagCallback> m_registeredTalkableTags;
	QMap<QString, ObjectTagCallback> m_registeredObjectTags;

	QString executeCmd(const QString &cmd);

	bool isActionParserTokenAtTop(const QStack<ParserToken> &parseStack, const QVector<ParserTokenType> &acceptedTokens);
	ParserToken parsePercentSyntax(const QString &s, int &idx, const Talkable &talkable, ParserEscape escape);

	template<typename ContainerClass>
	QString joinParserTokens(const ContainerClass &parseStack);

private slots:
	INJEQT_SET void setChatDataExtractor(ChatDataExtractor *chatDataExtractor);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);

};
