/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "chat-type.h"

bool ChatType::operator == (const ChatType &compare) const
{
	return name() == compare.name();
}

bool ChatType::operator < (const ChatType &compare) const
{
	return sortIndex() < compare.sortIndex();
}
