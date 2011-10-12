/*
 * %kadu copyright begin%
 * Copyright 201 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_TYPE_DATA_H
#define STATUS_TYPE_DATA_H

#include <QtCore/QString>
#include <QtCore/QVariant>

#include "status/status-type.h"
#include "status/status-type-group.h"

#include "exports.h"

class KADUAPI StatusTypeData
{
	StatusType Type;
	QString Name;
	QString DisplayName;
	QString IconName;

	StatusTypeGroup TypeGroup;

public:
	StatusTypeData();
	StatusTypeData(StatusType type, const QString &name, const QString &displayName, const QString &iconName, StatusTypeGroup typeGroup);
	StatusTypeData(const StatusTypeData &copyMe);

	StatusType type() const { return Type; }
	const QString & name() const { return Name; }
	const QString & displayName() const { return DisplayName; }
	const QString & iconName() const { return IconName; }
	StatusTypeGroup typeGroup() const { return TypeGroup; }

};

#endif // STATUS_TYPE_DATA_H
