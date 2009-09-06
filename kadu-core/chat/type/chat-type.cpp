/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "chat-type.h"

ChatType::ChatType(int sortIndex, const QString &name, const QString &displayName, const QIcon &icon) :
		SortIndex(sortIndex), Name(name), DisplayName(displayName), Icon(icon)
{
}

ChatType::ChatType(const ChatType &copy)
{
	SortIndex = copy.SortIndex;
	Name = copy.Name;
	DisplayName = copy.DisplayName;
	Icon = copy.Icon;
}

bool ChatType::operator == (const ChatType &compare)
{
	return Name == compare.Name;
}

bool ChatType::operator < (const ChatType &compare)
{
	return SortIndex < compare.SortIndex;
}
