/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/message/message-part.h"

#include "config_file.h"
#include "kadu_parser.h"
#include "misc/misc.h"

#include "chat_message.h"

QString formatMessage(const QString &text, const QString &backgroundColor)
{
	HtmlDocument htmlDocument;
	htmlDocument.parseHtml(text);
	htmlDocument.convertUrlsToHtml();
	htmlDocument.convertMailToHtml();
	EmoticonsManager::instance()->expandEmoticons(htmlDocument, backgroundColor, (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));
// 	GaduImagesManager::setBackgroundsForAnimatedImages(htmlDocument, backgroundColor);

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

ChatMessage::ChatMessage(Account *account, const Contact &sender, const ContactSet &receivers, const QString &unformattedMessage,
		ChatMessageType type, QDateTime date, QDateTime sdate)
	: SenderAccount(account), Sender(sender), Receivers(receivers), Date(date), SDate(sdate), Type(type)
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

	this->unformattedMessage.replace("\r\n", "<br/>");
	this->unformattedMessage.replace("\n",   "<br/>");
	this->unformattedMessage.replace("\r",   "<br/>");
	this->unformattedMessage.replace(QChar::LineSeparator, "<br />");

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
		EmoticonsManager::instance()->expandEmoticons(doc, bgcolor, style);

// 	GaduImagesManager::setBackgroundsForAnimatedImages(doc, bgcolor);
	edit = doc.generateHtml();

	return edit;
}

void ChatMessage::replaceLoadingImages(const QString &imageId, const QString &imagePath)
{
	unformattedMessage = MessagePart::replaceLoadingImages(unformattedMessage, imageId, imagePath);
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
