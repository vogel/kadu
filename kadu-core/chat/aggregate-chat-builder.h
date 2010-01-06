/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AGGREGATE_CHAT_BUILDER_H
#define AGGREGATE_CHAT_BUILDER_H

#include <QtCore/QObject>

#include "exports.h"

class AggregateChat;
class BuddySet;

class KADUAPI AggregateChatBuilder
{
public:
	static AggregateChat buildAggregateChat(BuddySet buddies);

};

#endif // AGGREGATE_CHAT_BUILDER_H
