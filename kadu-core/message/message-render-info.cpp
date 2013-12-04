/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QRegExp>

#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "message/message-html-renderer-service.h"
#include "misc/date-time.h"
#include "misc/misc.h"
#include "parser/parser.h"
#include "protocols/services/chat-image-service.h"
#include "url-handlers/url-handler-manager.h"

#include "message-render-info.h"

static QString getMessage(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return Core::instance()->messageHtmlRendererService()->renderMessage(messageRenderInfo->message());
	else
		return QString();
}

static QString getMessageId(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->message().id();
	return QString();
}

static QString getMessageStatus(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return QString::number(messageRenderInfo->message().status());
	return QString();
}

static QString getBackgroundColor(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->backgroundColor();
	else
		return QString();
}

static QString getFontColor(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->fontColor();
	else
		return QString();
}

static QString getNickColor(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return messageRenderInfo->nickColor();
	else
		return QString();
}

static QString getSentDate(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo && messageRenderInfo->showServerTime())
		return printDateTime(messageRenderInfo->message().sendDate());
	else
		return QString();
}

static QString getReceivedDate(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return printDateTime(messageRenderInfo->message().receiveDate());
	else
		return QString();
}

static QString getSeparator(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
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

MessageRenderInfo::MessageRenderInfo(const Message &msg) :
		MyMessage(msg), ShowServerTime(true)
{
	switch (msg.type())
	{
		case MessageTypeSent:
			BackgroundColor = ChatConfigurationHolder::instance()->myBackgroundColor();
			NickColor = ChatConfigurationHolder::instance()->myNickColor();
			FontColor = ChatConfigurationHolder::instance()->myFontColor();
			break;

		case MessageTypeReceived:
			BackgroundColor = ChatConfigurationHolder::instance()->usrBackgroundColor();
			NickColor = ChatConfigurationHolder::instance()->usrNickColor();
			FontColor = ChatConfigurationHolder::instance()->usrFontColor();
			break;

		default:
			// do nothing
			break;
	}
}

MessageRenderInfo::~MessageRenderInfo()
{
}

MessageRenderInfo & MessageRenderInfo::setShowServerTime(bool noServerTime, int noServerTimeDiff)
{
	ShowServerTime = (MyMessage.sendDate().isValid()
			&& (!noServerTime || (abs(static_cast<int>(MyMessage.receiveDate().toTime_t()) - static_cast<int>(MyMessage.sendDate().toTime_t()))) > noServerTimeDiff));
	return *this;
}

MessageRenderInfo & MessageRenderInfo::setSeparatorSize(int separatorSize)
{
	SeparatorSize = separatorSize;
	return *this;
}

MessageRenderInfo & MessageRenderInfo::setBackgroundColor(const QString &backgroundColor)
{
	BackgroundColor = backgroundColor;
	return *this;
}

MessageRenderInfo & MessageRenderInfo::setFontColor(const QString &fontColor)
{
	FontColor = fontColor;
	return *this;
}

MessageRenderInfo & MessageRenderInfo::setNickColor(const QString &nickColor)
{
	NickColor = nickColor;
	return *this;
}

#include "moc_message-render-info.cpp"
