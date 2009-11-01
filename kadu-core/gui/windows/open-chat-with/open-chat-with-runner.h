/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OPEN_CHAT_WITH_RUNNER_H
#define OPEN_CHAT_WITH_RUNNER_H

#include "buddies/buddy-list.h"

class KADUAPI OpenChatWithRunner
{

public:
	virtual BuddyList matchingContacts(const QString &query) = 0;

};

#endif // OPEN_CHAT_WITH_RUNNER_H
