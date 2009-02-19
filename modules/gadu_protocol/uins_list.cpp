/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "uins_list.h"

bool UinsList::equals(const UinsList &uins) const
{
	if (count() != uins.count())
		return false;

	foreach (const UinType &uin, *this)
		if (!uins.contains(uin))
			return false;

	return true;
}

UinsList::UinsList()
{
}

UinsList::UinsList(UinType uin)
{
	append(uin);
}

UinsList::UinsList(const QString &uins)
{
	QStringList list = uins.split(",", QString::SkipEmptyParts);
	foreach(const QString &uin, list)
		append(uin.toUInt());
}

UinsList::UinsList(const QStringList &list)
{
	foreach(const QString &uin, list)
		append(uin.toUInt());
}

void UinsList::sort()
{
	qSort(*this);
}

QStringList UinsList::toStringList() const
{
	QStringList list;
	foreach(const UinType &uin, *this)
		list.append(QString::number(uin));
	return list;
}

bool UinsList::operator < (const UinsList &compareTo) const
{
	if (count() < compareTo.count())
		return true;

	if (count() > compareTo.count())
		return false;

	for (int i = 0; i < count(); i++)
	{
		if (at(i) < compareTo.at(i))
			return true;
		if (compareTo.at(i) < at(i))
			return false;
	}

	return false;
}
