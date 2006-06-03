/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_message.h"

#include <qregexp.h>
#include "chat_styles.h"
#include "gadu_images_manager.h"
#include "misc.h"

ChatMessage::ChatMessage(const QString &nick_, const QString &unformattedMessage_, bool myMessage, QDateTime date_, QDateTime sdate_)
	: nick(nick_), date(date_), sdate(sdate_), unformattedMessage(unformattedMessage_), isMyMessage(myMessage),
	Colors(QColor(), QColor(), QColor()), attributes(), needsToBeFormatted(true), message()
{
}

ChatMessage::ChatMessage(const QString &formattedMessage, const ChatColors& colors)
	: nick(), date(), sdate(), unformattedMessage(), isMyMessage(true),
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

	// detekcja adresow url
	doc.convertUrlsToHtml();

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
	if (include_header)
	{
		QString escaped_nick = nick;
		QString date_str = printDateTime(date);
		QString date_str_2 = date_str;
		if (!sdate.isNull())
			date_str_2.append(" / S " + printDateTime(sdate));
		HtmlDocument::escapeText(escaped_nick);
		message = narg(chat_style->formatStringFull(),
			Colors.backgroundColor().name(),
			Colors.fontColor().name(),
			Colors.nickColor().name(),
			escaped_nick,
			date_str,
			date_str_2,
			convertCharacters(unformattedMessage, Colors.backgroundColor(), emoticons_style),
			QString::number(separator_size));
	}
	else if (separator_size > 0)
		message = narg(chat_style->formatStringPure(),
			Colors.backgroundColor().name(),
			Colors.fontColor().name(),
			Colors.nickColor().name(),
			QString::number(separator_size),
			convertCharacters(unformattedMessage, Colors.backgroundColor(), emoticons_style));
	else
		message = narg(chat_style->formatStringWithoutSeparator(),
			Colors.backgroundColor().name(),
			Colors.fontColor().name(),
			Colors.nickColor().name(),
			convertCharacters(unformattedMessage, Colors.backgroundColor(), emoticons_style));
	needsToBeFormatted = false;
}
