/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_GROUP_MANAGER_H
#define STATUS_GROUP_MANAGER_H

#include <QtCore/QList>
#include <QtCore/QMap>

#include "exports.h"

class StatusGroup;

class KADUAPI StatusGroupManager
{
	Q_DISABLE_COPY(StatusGroupManager)

	static StatusGroupManager *Instance;

	QList<StatusGroup *> StatusGroups;
	QMap<StatusGroup *, int> StatusGroupsCounter;

	StatusGroupManager();
	~StatusGroupManager();

public:
	static StatusGroupManager * instance();

	void registerStatusGroup(const QString &name, int sortIndex);
	void unregisterStatusGroup(const QString &name);

	StatusGroup * statusGroup(const QString &name);

};

#endif // STATUS_GROUP_MANAGER_H
