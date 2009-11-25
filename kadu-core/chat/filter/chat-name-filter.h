 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_NAME_FILTER_H
#define CHAT_NAME_FILTER_H

#include "chat/filter/chat-filter.h"
#include "exports.h"

class BuddyNameFilter;

class KADUAPI ChatNameFilter : public ChatFilter
{
	QString Name;
	BuddyNameFilter *Filter;

public:
	ChatNameFilter(QObject *parent = 0);

	virtual bool acceptChat(Chat chat);

	void setName(const QString &name);

};

#endif // CHAT_NAME_FILTER_H
