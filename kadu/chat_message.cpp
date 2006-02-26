/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_message.h"

ChatMessage::ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate)
{
	needsToBeFormatted=true;
	this->nick=nick;
	this->unformattedMessage=unformattedMessage;
	this->isMyMessage=myMessage;
	this->date=date;
	this->sdate=sdate;
}

ChatMessage::ChatMessage(const QString &formattedMessage, const QColor &bgColor, const QColor &txtColor, const QColor &nicColor)
{
	needsToBeFormatted=false;
	message=formattedMessage;
	backgroundColor=bgColor;
	textColor=txtColor;
	nickColor=nicColor;
}
