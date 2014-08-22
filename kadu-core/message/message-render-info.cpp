/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include "message-render-info.h"

#include "core/core.h"
#include "message/message-html-renderer-service.h"
#include "misc/date-time.h"
#include "parser/parser.h"

static QString getMessage(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return Core::instance()->messageHtmlRendererService()->renderMessage(messageRenderInfo->message());
	else
		return QString();
}

static QString getMessageId(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->message().id();
	return QString();
}

static QString getMessageStatus(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return QString::number(messageRenderInfo->message().status());
	return QString();
}

static QString getBackgroundColor(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->backgroundColor();
	else
		return QString();
}

static QString getFontColor(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->fontColor();
	else
		return QString();
}

static QString getNickColor(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->nickColor();
	else
		return QString();
}

static QString getSentDate(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo && messageRenderInfo->showServerTime())
		return printDateTime(messageRenderInfo->message().sendDate());
	else
		return QString();
}

static QString getReceivedDate(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return printDateTime(messageRenderInfo->message().receiveDate());
	else
		return QString();
}

static QString getSeparator(const ParserData * const object)
{
	const MessageRenderInfo * const messageRenderInfo = dynamic_cast<const MessageRenderInfo * const>(object);
	if (!messageRenderInfo)
		return QString();

	int separatorSize = messageRenderInfo->separatorSize();
	if (separatorSize)
		return "<div style=\"margin: 0; margin-top:" + QString::number(separatorSize) + "px\"></div>";
	else
		return QString();
}

void MessageRenderInfo::registerParserTags()
{
	Parser::registerObjectTag("message", getMessage);
	Parser::registerObjectTag("messageId", getMessageId);
	Parser::registerObjectTag("messageStatus", getMessageStatus);
	Parser::registerObjectTag("backgroundColor", getBackgroundColor);
	Parser::registerObjectTag("fontColor", getFontColor);
	Parser::registerObjectTag("nickColor", getNickColor);
	Parser::registerObjectTag("sentDate", getSentDate);
	Parser::registerObjectTag("receivedDate", getReceivedDate);
	Parser::registerObjectTag("separator", getSeparator);
}

void MessageRenderInfo::unregisterParserTags()
{
	Parser::unregisterObjectTag("message");
	Parser::unregisterObjectTag("messageId");
	Parser::unregisterObjectTag("messageStatus");
	Parser::unregisterObjectTag("backgroundColor");
	Parser::unregisterObjectTag("fontColor");
	Parser::unregisterObjectTag("nickColor");
	Parser::unregisterObjectTag("sentDate");
	Parser::unregisterObjectTag("receivedDate");
	Parser::unregisterObjectTag("separator");
}

MessageRenderInfo::MessageRenderInfo(Message message, QString backgroundColor, QString fontColor, QString nickColor,
			bool includeHeader, int separatorSize, bool showServerTime) :
		m_message{std::move(message)}, m_backgroundColor{std::move(backgroundColor)}, m_fontColor{std::move(fontColor)}, m_nickColor{std::move(nickColor)},
		m_includeHeader(includeHeader), m_separatorSize{separatorSize}, m_showServerTime{showServerTime}
{
}

MessageRenderInfo::~MessageRenderInfo()
{
}

Message MessageRenderInfo::message() const
{
	return m_message;
}

QString MessageRenderInfo::backgroundColor() const
{
	return m_backgroundColor;
}

QString MessageRenderInfo::fontColor() const
{
	return m_fontColor;
}

QString MessageRenderInfo::nickColor() const
{
	return m_nickColor;
}

bool MessageRenderInfo::includeHeader() const
{
	return m_includeHeader;
}

int MessageRenderInfo::separatorSize() const
{
	return m_separatorSize;
}

bool MessageRenderInfo::showServerTime() const
{
	return m_showServerTime;
}
