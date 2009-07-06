/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_TYPE_MANAGER
#define STATUS_TYPE_MANAGER

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>

#include "exports.h"

class StatusGroup;
class StatusType;

class KADUAPI StatusTypeManager
{
	Q_DISABLE_COPY(StatusTypeManager)

	static StatusTypeManager *Instance;

	QList<StatusType *> StatusTypes;
	QMap<StatusType *, int> StatusTypesCounter;

	StatusTypeManager();

public:
	static StatusTypeManager * instance();

	void registerStatusType(const QString &name, const QString &displayName, StatusGroup *statusGroup, int sortIndex);
	void unregisterStatusType(const QString &name);

	StatusType * statusType(const QString &name);

};

#endif // STATUS_TYPE_MANAGER
