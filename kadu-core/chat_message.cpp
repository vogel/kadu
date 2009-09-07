/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "gadu_images_manager.h"
#include "kadu_parser.h"
#include "misc.h"

#include "chat_message.h"

QString formatMessage(const QString &text, const QString &backgroundColor)
{
	HtmlDocument htmlDocument;
	htmlDocument.parseHtml(text);
	htmlDocument.convertUrlsToHtml();
	htmlDocument.convertMailToHtml();
	emoticons->expandEmoticons(htmlDocument, backgroundColor, (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
	GaduImagesManager::setBackgroundsForAnimatedImages(htmlDocument, backgroundColor);

	return htmlDocument.generateHtml();
}

static QString getMessage(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return formatMessage(chatMessage->unformattedMessage, chatMessage->backgroundColor);
	else
		return "";
}

static QString getBackgroundColor(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return chatMessage->backgroundColor;
	else
		return "";
}

static QString getFontColor(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return chatMessage->fontColor;
	else
		return "";
}

static QString getNickColor(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return chatMessage->nickColor;
	else
		return "";
}

static QString getSentDate(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return chatMessage->sentDate;
	else
		return "";
}

static QString getReceivedDate(const QObject * const object)
{
	const ChatMessage * const chatMessage = dynamic_cast<const ChatMessage * const>(object);
	if (chatMessage)
		return chatMessage->receivedDate;
	else
		return "";
}

static QString getSeparator(const QObject * const object)
{
	int separatorSize = dynamic_cast<const ChatMessage * const>(object)->separatorSize();

	if (separatorSize)
		return "<div style=\"margin: 0; margin-top:" + QString::number(separatorSize) + "px\"></div>";
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

ChatMessage::ChatMessage(const UserListElement &ule, const UserListElements &receivers, const QString &unformattedMessage, ChatMessageType type, QDateTime date, QDateTime sdate)
	: Ule(ule), Receivers(receivers), Date(date), SDate(sdate), Type(type)
{
	receivedDate = printDateTime(date);

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

	this->unformattedMessage = unformattedMessage;
// 	convertCharacters(unformattedMessage, backgroundColor,
// 		(EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
}

ChatMessage::ChatMessage(const QString &rawContent, ChatMessageType type, QDateTime date,
	QString backgroundColor, QString fontColor, QString nickColor)
	: Date(date), Type(type), unformattedMessage(rawContent), backgroundColor(backgroundColor), fontColor(fontColor), nickColor(nickColor)
{
}

// TODO: remove?
/* convert special characters into emoticons, HTML into plain text and so forth */
QString ChatMessage::convertCharacters(QString edit, const QColor &bgcolor, EmoticonsStyle style)
{
	// zmieniamy windowsowe \r\n na unixowe \n
	edit.replace("\r\n", "<br/>");
	edit.replace("\n",   "<br/>");
	edit.replace("\r",   "<br/>");
	edit.replace(QChar::LineSeparator, "<br />");

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

void ChatMessage::replaceLoadingImages(UinType sender, quint32 size, quint32 crc32)
{
	unformattedMessage = gadu_images_manager.replaceLoadingImages(unformattedMessage, sender, size, crc32);
}

void ChatMessage::setShowServerTime(bool noServerTime, int noServerTimeDiff)
{
	if (SDate.isValid() && (!noServerTime || (abs(Date.toTime_t()-SDate.toTime_t())) > noServerTimeDiff))
		sentDate = printDateTime(SDate);
	else
		sentDate = QString::null;
}

void ChatMessage::setColorsAndBackground(QString &backgroundColor, QString &nickColor, QString &fontColor)
{
		if (Type == TypeSystem)
			return;

		this->backgroundColor = backgroundColor;
		this->nickColor = nickColor;
		this->fontColor = fontColor;
}
