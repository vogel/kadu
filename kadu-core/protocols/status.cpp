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

Status::StatusType Status::comparableType(Status::StatusType type)
{
	if (Busy == type)
		return Online;

	return type;
}

int Status::compareTo(const Status &compare) const
{
	return comparableType(compare.Type) - comparableType(Type);
}
