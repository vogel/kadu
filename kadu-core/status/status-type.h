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

class StatusGroup;

class StatusType
{
	QString Name;
	QString DisplayName;

	StatusGroup *MyStatusGroup;

	int SortIndex;

public:
	StatusType(const QString &name, const QString &displayName, StatusGroup *statusType, int sortIndex);

	QString name() { return Name; }
	QString displayName() { return DisplayName; }

	StatusGroup * statusGroup() { return MyStatusGroup; }

	int sortIndex() { return SortIndex; }

	bool operator < (const StatusType &compare) const;
	bool operator == (const StatusType &compare) const;
	bool operator != (const StatusType &compare) const;

};

#endif // STATUS_TYPE
