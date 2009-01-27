/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status.h"

Status::Status(Status::StatusType type, QString description)
	: Type(type), Description(description)
{
}

Status::Status(const Status& copyme)
	: Type(copyme.Type), Description(copyme.Description)
{
}

QString Status::name(const Status &status, bool fullName)
{
	QString add((fullName && !status.Description.isNull()) ? "WithDescription" : "");

	switch (status.Type)
	{
		case Online:
			return QString("Online").append(add);
		case Busy:
			return QString("Busy").append(add);
		case Invisible:
			return QString("Invisible").append(add);
		default:
			return QString("Offline").append(add);
	}
}

Status::StatusType Status::comparableType(Status::StatusType type)
{
	if (Busy == type)
		return Online;

	return type;
}

int Status::compareTo(const Status &compare) const
{
	return comparableType(Type) - comparableType(compare.Type);
}

bool Status::operator < (const Status &compare) const
{
	return compareTo(compare) < 0;
}
