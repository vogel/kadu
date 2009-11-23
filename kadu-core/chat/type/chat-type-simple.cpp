/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "icons-manager.h"

#include "chat-type-simple.h"

int ChatTypeSimple::sortIndex() const
{
	return 0;
}

QString ChatTypeSimple::name() const
{
	return "SimpleChat";
}

QString ChatTypeSimple::displayName() const
{
	return tr("Chat");
}

QIcon ChatTypeSimple::icon() const
{
	return IconsManager::instance()->loadIcon("OpenChat");
}
