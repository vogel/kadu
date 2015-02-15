/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "exports.h"

#include <memory>

class StoragePoint;
class RosterTask;

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @class RosterTaskCollectionStorage
 * @short Stores and loads collection of RosterTask objects from Storage.
  */
class KADUAPI RosterTaskCollectionStorage
{

public:
	/**
	 * @pre storage != nullptr && storage->storage() != nullptr
	 */
	RosterTaskCollectionStorage(std::shared_ptr<StoragePoint> storage);

	/**
	 * @return List of tasks found in storage passed to constructor
	 */
	QVector<RosterTask> loadRosterTasks();

	/**
	 * @short Stores @p tasks in storage passed to constructor.
	 */
	void storeRosterTasks(const QVector<RosterTask> &tasks);

private:
	std::shared_ptr<StoragePoint> m_storage;

};

/**
 * @}
 */
