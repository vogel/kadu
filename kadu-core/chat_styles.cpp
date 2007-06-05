/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_styles.h"
#include "config_file.h"

ChatStyle::ChatStyle(const QString& format_string_full,
			const QString& format_string_pure)
	: FormatStringFull(format_string_full),
	  FormatStringPure(format_string_pure),
	  noServerTime(config_file.readBoolEntry("Look", "NoServerTime")),
	  noServerTimeDiff(config_file.readNumEntry("Look", "NoServerTimeDiff"))
{
}

ChatStyle::ChatStyle(const QString& format_string_full)
	: FormatStringFull(format_string_full),
	  FormatStringPure("<p style=\"background-color: #{backgroundColor}\"><kadu:separator/><font color=\"#{fontColor}\">#{message}</font></p>"),
	  noServerTime(config_file.readBoolEntry("Look", "NoServerTime")),
	  noServerTimeDiff(config_file.readNumEntry("Look", "NoServerTimeDiff"))
{
}

ChatStyle::~ChatStyle()
{
}

const bool ChatStyle::noServerTimes() const
{
	return noServerTime;
}

const int ChatStyle::noServerTimesDiff() const
{
	return noServerTimeDiff;
}

const QString& ChatStyle::formatStringFull() const
{
	return FormatStringFull;
}

const QString& ChatStyle::formatStringPure() const
{
	return FormatStringPure;
}

KaduChatStyle::KaduChatStyle()
	: ChatStyle("<p style=\"background-color: #{backgroundColor}\"><kadu:separator/><font color=\"#{fontColor}\"><b><font color=\"#{nickColor}\">%a</font> :: #{receivedDate}[ / S #{sentDate}]</b><br/>#{message}</font></p>")
{
}

HapiChatStyle::HapiChatStyle()
	: ChatStyle("<p style=\"background-color: #{backgroundColor}\"><kadu:separator/>\n"\
				"	<table style=\"border-bottom: solid 1px black;\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">\n"\
				"	<tr><td align=\"left\" width=\"50%\" valign=\"bottom\"><b><font color=\"#{nickColor}\">%a</font></b></td>\n"\
				"	<td align=\"right\" width=\"50%\"><font color=\"#{fontColor}\">#{receivedDate}[ / S #{sentDate]]</font></td></tr></table>\n"\
				"<hr/><font color=\"#{fontColor}\">#{message}</font></p>")
{
}

IrcChatStyle::IrcChatStyle()
	: ChatStyle("<p style=\"background-color: #{backgroundColor}\"><kadu:separator/><font color=\"#{fontColor}\"><b>[#{sentDate}] <font color=\"#{nickColor}\">%a</font>: </b> #{message}</font></p>")
{
}

CustomChatStyle::CustomChatStyle(const QString& full_style)
	: ChatStyle("<p style=\"background-color: #{backgroundColor}\"><kadu:separator/>" + full_style + "</p>")
{
}
