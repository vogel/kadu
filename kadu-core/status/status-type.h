/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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
	QString IconName;

	StatusGroup *MyStatusGroup;

	int SortIndex;

public:
	static bool lessThan(const StatusType *left, const StatusType *right);

	StatusType(const QString &name, const QString &displayName, const QString &iconName, StatusGroup *statusType, int sortIndex);

	const QString & name() const { return Name; }
	const QString & displayName() const { return DisplayName; }
	const QString & iconName() const { return IconName; }

	StatusGroup *statusGroup() { return MyStatusGroup; }

	int sortIndex() { return SortIndex; }

	bool operator < (const StatusType &compare) const;
	bool operator == (const StatusType &compare) const;
	bool operator != (const StatusType &compare) const;

};

Q_DECLARE_METATYPE(StatusType *)

#endif // STATUS_TYPE
