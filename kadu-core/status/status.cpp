/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status/status-type-data.h"
#include "status/status-type-group.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"

#include "status.h"

#include <stdio.h>

Status::Status(StatusType statusType, const QString &description) :
		Description(description)
{
	setType(statusType);
}

Status::Status(const Status& copyme) :
		Type(copyme.Type), Group(copyme.Group), Description(copyme.Description),
		DisplayName(copyme.DisplayName)
{
}

Status::~Status()
{
}

void Status::setType(StatusType type)
{
	Type = type;

	const StatusTypeData & typeData = StatusTypeManager::instance()->statusTypeData(Type);
	DisplayName = typeData.displayName();
	Group = typeData.typeGroup();
}

bool Status::isDisconnected() const
{
	return StatusTypeGroupOffline == Group;
}

bool Status::operator < (const Status &compare) const
{
	return Type < compare.Type;
}

bool Status::operator == (const Status &compare) const
{
	return Type == compare.Type
	       && Description == compare.Description;
}

bool Status::operator != (const Status& compare) const
{
	return !(*this == compare);
}

bool Status::hasDescription() const
{
	return !Description.isEmpty();
}
