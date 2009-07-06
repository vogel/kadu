/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_GROUP_MANAGER
#define STATUS_GROUP_MANAGER

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

public:
	static StatusGroupManager * instance();

	void registerStatusGroup(const QString &name, int sortIndex);
	void unregisterStatusGroup(const QString &name);

	StatusGroup * statusGroup(const QString &name);

};

#endif // STATUS_GROUP_MANAGER
