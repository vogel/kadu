/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status/status-group.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "status.h"

Status::Status(const QString &type, const QString &description)
	: Description(description)
{
	setType(type);
}

Status::Status(const Status& copyme)
	: Type(copyme.Type), Group(copyme.Group), Description(copyme.Description)
{
}

void Status::setType(const QString& type)
{
	Group = "Offline";
	Type = type;

	StatusType *statusType = StatusTypeManager::instance()->statusType(Type);
	if (!statusType)
		return;

	StatusGroup *statusGroup = statusType->statusGroup();
	if (!statusGroup)
		return;

	Group = statusGroup->name();
}

QString Status::name(const Status &status, bool fullName)
{
	QString add((fullName && !status.Description.isEmpty()) ? "WithDescription" : "");
	return status.Type + add;
}

bool Status::isDisconnected() const
{
	return "Offline" == Type;
}

bool Status::operator < (const Status &compare) const
{
	StatusType *left = StatusTypeManager::instance()->statusType(Type);
	StatusType *right = StatusTypeManager::instance()->statusType(compare.Type);

	if (!left)
		return true;

	if (!right)
		return false;

	return *left < *right;
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
