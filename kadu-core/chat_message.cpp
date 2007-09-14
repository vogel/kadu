/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qregexp.h>

#include "kadu_parser.h"
#include "gadu_images_manager.h"
#include "misc.h"

#include "chat_message.h"

static QString getMessage(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return chatMessage->unformattedMessage;
	else
		return "";
}

static QString getBackgroundColor(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return dynamic_cast<const ChatMessage * const>(object)->backgroundColor;
	else
		return "";
}

static QString getFontColor(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return dynamic_cast<const ChatMessage * const>(object)->fontColor;
	else
		return "";
}

static QString getNickColor(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return dynamic_cast<const ChatMessage * const>(object)->nickColor;
	else
		return "";
}

static QString getSentDate(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return dynamic_cast<const ChatMessage * const>(object)->sentDate;
	else
		return "";
}

static QString getReceivedDate(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return dynamic_cast<const ChatMessage * const>(object)->receivedDate;
	else
		return "";
}

static QString getSeparator(const QObject * const object)
{
	int separatorSize = dynamic_cast<const ChatMessage * const>(object)->separatorSize();

	if (separatorSize)
		return "<img title=\"\" height=\"" + QString::number(separatorSize) + "\" width=\"10000\" align=\"right\">";
	else
		return "";
}

void ChatMessage::registerParserTags()
{
	KaduParser::registerObjectTag("message", getMessage);
	KaduParser::registerObjectTag("backgroundColor", getBackgroundColor);
	KaduParser::registerObjectTag("fontColor", getFontColor);
	KaduParser::registerObjectTag("nickColor", getNickColor);
	KaduParser::registerObjectTag("sentDate", getSentDate);
	KaduParser::registerObjectTag("receivedDate", getReceivedDate);
	KaduParser::registerObjectTag("separator", getSeparator);
}

void ChatMessage::unregisterParserTags()
{
	KaduParser::unregisterObjectTag("message", getMessage);
	KaduParser::unregisterObjectTag("backgroundColor", getBackgroundColor);
	KaduParser::unregisterObjectTag("fontColor", getFontColor);
	KaduParser::unregisterObjectTag("nickColor", getNickColor);
	KaduParser::unregisterObjectTag("sentDate", getSentDate);
	KaduParser::unregisterObjectTag("receivedDate", getReceivedDate);
	KaduParser::unregisterObjectTag("separator", getSeparator);
}

ChatMessage::ChatMessage(const UserListElement &ule, const QString &unformattedMessage, ChatMessageType type, QDateTime date, QDateTime sdate)
	: Ule(ule), Date(date), Type(type)
{
	receivedDate = printDateTime(date);
	sentDate = printDateTime(sdate);

	switch (type)
	{
		case TypeSent:
			backgroundColor = config_file.readEntry("Look", "ChatMyBgColor");
			fontColor = config_file.readEntry("Look", "ChatMyFontColor");
			nickColor = config_file.readEntry("Look", "ChatMyNickColor");
			break;

		case TypeReceived:
			backgroundColor = config_file.readEntry("Look", "ChatUsrBgColor");
			fontColor = config_file.readEntry("Look", "ChatUsrFontColor");
			nickColor = config_file.readEntry("Look", "ChatUsrNickColor");
			break;

		case TypeSystem:
			break;
	}

	this->unformattedMessage = convertCharacters(unformattedMessage, backgroundColor,
		(EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
}

ChatMessage::ChatMessage(const QString &rawContent, ChatMessageType type, QDateTime date,
	QString backgroundColor, QString fontColor, QString nickColor)
	: unformattedMessage(rawContent), backgroundColor(backgroundColor), fontColor(fontColor), nickColor(nickColor), Date(date), Type(type)
{
}

/* convert special characters into emoticons, HTML into plain text and so forth */
QString ChatMessage::convertCharacters(QString edit, const QColor &bgcolor, EmoticonsStyle style)
{
	// zmieniamy windowsowe \r\n na unixowe \n
	edit.replace("\r\n", "<br/>");
	edit.replace("\n",   "<br/>");

	HtmlDocument doc;
	doc.parseHtml(edit);

	// detekcja adresow url i email
	doc.convertGGToHtml();
	doc.convertUrlsToHtml();
	doc.convertMailToHtml();

	if (style != EMOTS_NONE)
		emoticons->expandEmoticons(doc, bgcolor, style);

	GaduImagesManager::setBackgroundsForAnimatedImages(doc, bgcolor);
	edit = doc.generateHtml();

	// workaround for bug in Qt - if there's a space after image, Qt does not show it, so we are replacing it with &nbsp;
	// regular expression has to contain "title", because this attribute may contain ">" (as in emoticon <rotfl>)
	const static QRegExp emotRegExp("<img emoticon=\"([01])\" title=\"([^\"]*)\" ([^>]*)> ");
	const static QString emotAfter ("<img emoticon=\"\\1\" title=\"\\2\" \\3>&nbsp;");
	edit.replace(emotRegExp, emotAfter);
	const static QRegExp imageRegExp("<img src=\"([^\"]*)\"([^>]*)> ");
	const static QString imageAfter( "<img src=\"\\1\"\\2>&nbsp;");
	edit.replace(imageRegExp, imageAfter);

	return edit;
}

void ChatMessage::replaceLoadingImages(UinType sender, uint32_t size, uint32_t crc32)
{
	unformattedMessage = gadu_images_manager.replaceLoadingImages(unformattedMessage, sender, size, crc32);
}
