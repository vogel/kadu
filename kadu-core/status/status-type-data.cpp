/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-type-data.h"

StatusTypeData::StatusTypeData() :
		Type{},
		TypeGroup{}
{
}

StatusTypeData::StatusTypeData(StatusType type, const QString &name, const QString &displayName, const QString &iconName, StatusTypeGroup typeGroup) :
		Type(type), Name(name), DisplayName(displayName), IconName(iconName), TypeGroup(typeGroup)
{
}

StatusTypeData::StatusTypeData(const StatusTypeData &copyMe) :
		Type(copyMe.Type), Name(copyMe.Name), DisplayName(copyMe.DisplayName), IconName(copyMe.IconName), TypeGroup(copyMe.TypeGroup)
{
}
