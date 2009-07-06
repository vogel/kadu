/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_GROUP
#define STATUS_GROUP

#include <QtCore/QString>

class StatusGroup
{
public:
	enum StatusGroupSortIndex
	{
		StatusGroupSortIndexAfterSetDescription = 101
	};

private:
	QString Name;
	int SortIndex;

public:
	StatusGroup(const QString &name, int sortIndex);

	QString name() { return Name; }

	int sortIndex() { return SortIndex; }

	bool operator < (const StatusGroup &compare) const;
	bool operator == (const StatusGroup &compare) const;
	bool operator != (const StatusGroup &compare) const;

};

#endif // STATUS_GROUP
