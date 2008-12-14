/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account_data.h"

AccountData::AccountData()
{
}

AccountData::AccountData(const QString &id, const QString &password)
{
	setId(id);
	setPassword(password);
}

AccountData::~AccountData()
{
}

bool AccountData::setId(const QString &id)
{
	if (!validateId(id))
		return false;

	Id = id;
	return true;
}
