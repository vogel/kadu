/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_TYPE
#define STATUS_TYPE

#include <QtCore/QString>
#include <QtCore/QVariant>

#include "exports.h"

class StatusGroup;

class KADUAPI StatusType
{
	QString Name;
	QString DisplayName;

	StatusGroup *MyStatusGroup;

	int SortIndex;

public:
	static bool lessThan(const StatusType *left, const StatusType *right);

	StatusType(const QString &name, const QString &displayName, StatusGroup *statusType, int sortIndex);

	QString name() { return Name; }
	QString displayName() { return DisplayName; }

	StatusGroup * statusGroup() { return MyStatusGroup; }

	int sortIndex() { return SortIndex; }

	bool operator < (const StatusType &compare) const;
	bool operator == (const StatusType &compare) const;
	bool operator != (const StatusType &compare) const;

};

Q_DECLARE_METATYPE(StatusType *)

#endif // STATUS_TYPE
