/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "chat/message/formatted-message-part.h"
#include "configuration/configuration-file.h"
#include "emoticons/emoticons-manager.h"
#include "icons-manager.h"
#include "misc/misc.h"
#include "parser/parser.h"
#include "protocols/services/chat-image-service.h"
#include "url-handlers/url-handler-manager.h"

#include "message-render-info.h"

QString formatMessage(const QString& text)
{
	HtmlDocument htmlDocument;
	htmlDocument.parseHtml(text);
	UrlHandlerManager::instance()->convertAllUrls(htmlDocument);
	EmoticonsManager::instance()->expandEmoticons(htmlDocument, (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));

	return htmlDocument.generateHtml();
}

static QString getMessage(const QObject * const object)
{
	const MessageRenderInfo * const messageRenderInfo = qobject_cast<const MessageRenderInfo * const>(object);
	if (messageRenderInfo)
		return formatMessage(messageRenderInfo->htmlMessageContent());
	else
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

static QString loadingImageHtml(const QString &imageId)
{
	return QString("<img src=\"file:///%1\" id=\"%2\" />")
			.arg(IconsManager::instance()->iconPath("kadu_icons/16x16/please-wait.gif"))
			.arg(imageId);
}

void MessageRenderInfo::registerParserTags()
{
	Parser::registerObjectTag("message", getMessage);
	Parser::registerObjectTag("backgroundColor", getBackgroundColor);
	Parser::registerObjectTag("fontColor", getFontColor);
	Parser::registerObjectTag("nickColor", getNickColor);
	Parser::registerObjectTag("sentDate", getSentDate);
	Parser::registerObjectTag("receivedDate", getReceivedDate);
	Parser::registerObjectTag("separator", getSeparator);
}

void MessageRenderInfo::unregisterParserTags()
{
	Parser::unregisterObjectTag("message", getMessage);
	Parser::unregisterObjectTag("backgroundColor", getBackgroundColor);
	Parser::unregisterObjectTag("fontColor", getFontColor);
	Parser::unregisterObjectTag("nickColor", getNickColor);
	Parser::unregisterObjectTag("sentDate", getSentDate);
	Parser::unregisterObjectTag("receivedDate", getReceivedDate);
	Parser::unregisterObjectTag("separator", getSeparator);
}

MessageRenderInfo::MessageRenderInfo(const Message &msg) :
		MyMessage(msg), ShowServerTime(true)
{
	switch (msg.type())
	{
		case Message::TypeSent:
			BackgroundColor = config_file.readEntry("Look", "ChatMyBgColor");
			FontColor = config_file.readEntry("Look", "ChatMyFontColor");
			NickColor = config_file.readEntry("Look", "ChatMyNickColor");
			break;

		case Message::TypeReceived:
			BackgroundColor = config_file.readEntry("Look", "ChatUsrBgColor");
			FontColor = config_file.readEntry("Look", "ChatUsrFontColor");
			NickColor = config_file.readEntry("Look", "ChatUsrNickColor");
			break;

		default:
			// do nothing
			break;
	}

	HtmlMessageContent = replacedNewLine(MyMessage.content(), QLatin1String("<br/>"));
	HtmlMessageContent.replace(QChar::LineSeparator, QLatin1String("<br/>"));

	// compare this regexp with FormattedMessagePart::toHtml()
	QRegExp kaduimgRegExp("<img src=\"kaduimg:///([^\"]*)\" />");
	int pos = 0;
	while ((pos = kaduimgRegExp.indexIn(HtmlMessageContent, pos)) != -1)
	{
		QString imgId = kaduimgRegExp.cap(1);
		// TODO 0.6.6: remove the first condition before RC release (will break compatibility with sent images saved in history by beta11)
		if (!imgId.startsWith('/') && !QFile(ChatImageService::imagesPath() + imgId).exists())
			HtmlMessageContent.replace(kaduimgRegExp.cap(0), loadingImageHtml(imgId));
		else
			pos += kaduimgRegExp.matchedLength();
	}

// 	convertCharacters(unformattedMessage, backgroundColor,
// 		(EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
}

MessageRenderInfo::~MessageRenderInfo()
{
}

void MessageRenderInfo::replaceLoadingImages(const QString &imageId, const QString &imageFileName)
{
	QString img = QString("<img src=\"kaduimg:///%1\" />").arg(imageFileName);
	HtmlMessageContent.replace(loadingImageHtml(imageId), img);
}

MessageRenderInfo & MessageRenderInfo::setShowServerTime(bool noServerTime, int noServerTimeDiff)
{
	ShowServerTime = (MyMessage.sendDate().isValid()
			&& (!noServerTime || (abs(MyMessage.receiveDate().toTime_t() - MyMessage.sendDate().toTime_t())) > noServerTimeDiff));
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
