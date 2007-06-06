/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qregexp.h>

#include "chat_styles.h"
#include "kadu_parser.h"
#include "gadu_images_manager.h"
#include "misc.h"

#include "chat_message.h"

static QString getMessage(const QObject * const object)
{
	return dynamic_cast<const ChatMessage * const>(object)->formattedMessage;
}

static QString getBackgroundColor(const QObject * const object)
{
	return dynamic_cast<const ChatMessage * const>(object)->backgroundColor;
}

static QString getFontColor(const QObject * const object)
{
	return dynamic_cast<const ChatMessage * const>(object)->fontColor;
}

static QString getNickColor(const QObject * const object)
{
	return dynamic_cast<const ChatMessage * const>(object)->nickColor;
}

static QString getSentDate(const QObject * const object)
{
	return dynamic_cast<const ChatMessage * const>(object)->sentDate;
}

static QString getReceivedDate(const QObject * const object)
{
	return dynamic_cast<const ChatMessage * const>(object)->receivedDate;
}

void ChatMessage::registerParserTags()
{
	KaduParser::registerObjectTag("message", getMessage);
	KaduParser::registerObjectTag("backgroundColor", getBackgroundColor);
	KaduParser::registerObjectTag("fontColor", getFontColor);
	KaduParser::registerObjectTag("nickColor", getNickColor);
	KaduParser::registerObjectTag("sentDate", getSentDate);
	KaduParser::registerObjectTag("receivedDate", getReceivedDate);
}

void ChatMessage::unregisterParserTags()
{
	KaduParser::unregisterObjectTag("message", getMessage);
	KaduParser::unregisterObjectTag("backgroundColor", getBackgroundColor);
	KaduParser::unregisterObjectTag("fontColor", getFontColor);
	KaduParser::unregisterObjectTag("nickColor", getNickColor);
	KaduParser::unregisterObjectTag("sentDate", getSentDate);
	KaduParser::unregisterObjectTag("receivedDate", getReceivedDate);
}

ChatMessage::ChatMessage(const UserListElement &ule, const QString &unformattedMessage_, bool myMessage, QDateTime date_, QDateTime sdate_)
	: ule(ule), date(date_), sdate(sdate_), unformattedMessage(unformattedMessage_), isMyMessage(myMessage),
	Colors(QColor(), QColor(), QColor()), attributes(), needsToBeFormatted(true), message()
{
}

ChatMessage::ChatMessage(const QString &formattedMessage, const ChatColors& colors)
	: ule(UserListElement()), date(), sdate(), unformattedMessage(), isMyMessage(true),
	Colors(colors), attributes(), needsToBeFormatted(false), message(formattedMessage)
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

void ChatMessage::formatMessage(const ChatStyle* chat_style,
	const EmoticonsStyle emoticons_style, bool include_header, int separator_size)
{

	formattedMessage = convertCharacters(unformattedMessage, Colors.backgroundColor(), emoticons_style);
	backgroundColor = Colors.backgroundColor().name();
	fontColor = Colors.fontColor().name();
	nickColor = Colors.nickColor().name();

	sentDate = "";
	receivedDate = "";

	QString fmt;
	if (include_header)
	{
		receivedDate = printDateTime(date);
		if (!sdate.isNull())
			if (!chat_style->noServerTimes() ||
		    	    (abs(date.toTime_t()-sdate.toTime_t()))>chat_style->noServerTimesDiff())
				sentDate = printDateTime(sdate);

		fmt = chat_style->formatStringFull();
		if (separator_size > 0)
			fmt.replace("<kadu:separator/>", QString("<img title=\"\" height=\"%1\" width=\"10000\" align=\"right\">").arg(separator_size));
		else
			fmt.remove("<kadu:separator/>");

		message = KaduParser::parse(fmt, ule, this);
	}
	else
	{
		fmt = chat_style->formatStringPure();
		if (separator_size > 0)
			fmt.replace("<kadu:separator/>", QString("<img title=\"\" height=\"%1\" width=\"10000\" align=\"right\">").arg(separator_size));
		else
			fmt.remove("<kadu:separator/>");

		printf("format is: %s\n", fmt.data());
		message = KaduParser::parse(fmt, ule, this);
	}
	needsToBeFormatted = false;
}
