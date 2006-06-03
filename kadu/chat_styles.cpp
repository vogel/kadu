/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_styles.h"

ChatStyle::ChatStyle(const QString& format_string_full,
			const QString& format_string_pure,
			const QString& format_string_without_separator)
	: FormatStringFull(format_string_full), FormatStringPure(format_string_pure),
		FormatStringWithoutSeparator(format_string_without_separator)
{
}

ChatStyle::~ChatStyle()
{
}

const QString& ChatStyle::formatStringFull() const
{
	return FormatStringFull;
}

const QString& ChatStyle::formatStringPure() const
{
	return FormatStringPure;
}

const QString& ChatStyle::formatStringWithoutSeparator() const
{
	return FormatStringWithoutSeparator;
}

KaduChatStyle::KaduChatStyle()
	: ChatStyle(
		"<p style=\"background-color: %1\"><img title=\"\" height=\"%8\" width=\"10000\" align=\"right\"><font color=\"%2\"><b><font color=\"%3\">%4</font> :: %6</b><br/>%7</font></p>",
		"<p style=\"background-color: %1\"><img title=\"\" height=\"%4\" width=\"10000\" align=\"right\"><font color=\"%2\">%5</font></p>",
		"<p style=\"background-color: %1\"><font color=\"%2\">%4</font></p>")
{
}

HapiChatStyle::HapiChatStyle()
	: ChatStyle(
		"<p style=\"background-color: %1;\"><img title=\"\" height=\"%8\" width=\"10000\" align=\"right\">"\
			"<table style=\"border-bottom: solid 1px black;\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">"\
			"<tr><td align=\"left\" width=\"50%\" valign=\"bottom\"><b><font color=\"%3\">%4</font></b></td>"\
			"<td align=\"right\" width=\"50%\"><font color=\"%2\">%6</font></td></tr></table>"\
		"<hr/><font color=\"%2\">%7</font></p>",
		"<p style=\"background-color: %1\"><img title=\"\" height=\"%4\" width=\"10000\" align=\"right\"><font color=\"%2\">%5</font></p>",
		"<p style=\"background-color: %1\"><font color=\"%2\">%4</font></span></p>")
{
}

IrcChatStyle::IrcChatStyle()
	: ChatStyle(
		"<p style=\"background-color: %1\"><img title=\"\" height=\"%8\" width=\"10000\" align=\"right\"><font color=\"%2\"><b>[%5] <font color=\"%3\">%4</font>: </b> %7</font></p>",
		"<p style=\"background-color: %1\"><img title=\"\" height=\"%4\" width=\"10000\" align=\"right\"><font color=\"%2\">%5</font></p>",
		"<p style=\"background-color: %1\"><font color=\"%2\">%4</font></p>")
{
}

CustomChatStyle::CustomChatStyle(const QString& full_style)
	: ChatStyle(
		"<p style=\"background-color: %1;\"><img title=\"\" height=\"%8\" width=\"10000\" align=\"right\">" + full_style + "</p>",
		"<p style=\"background-color: %1\"><img title=\"\" height=\"%4\" width=\"10000\" align=\"right\"><font color=\"%2\">%5</font></p>",
		"<p style=\"background-color: %1\"><font color=\"%2\">%4</font></p>")
{
}
