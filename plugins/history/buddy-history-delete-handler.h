/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BUDDY_HISTORY_DELETE_HANDLER_H
#define BUDDY_HISTORY_DELETE_HANDLER_H

#include "buddies/buddy-additional-data-delete-handler.h"

class BuddyHistoryDeleteHandler : public BuddyAdditionalDataDeleteHandler
{
	Q_DISABLE_COPY(BuddyHistoryDeleteHandler)

	static BuddyHistoryDeleteHandler *Instance;

	BuddyHistoryDeleteHandler();
	virtual ~BuddyHistoryDeleteHandler();

public:
	static void createInstance();
	static void destroyInstance();
	static BuddyHistoryDeleteHandler *instance();

	virtual QString name();
	virtual QString displayName();
	virtual void deleteBuddyAdditionalData(Buddy buddy);

};

#endif // BUDDY_HISTORY_DELETE_HANDLER_H
