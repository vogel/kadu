/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "icons-manager.h"

#include "chat-type-conference.h"

int ChatTypeConference::sortIndex() const
{
	return 1;
}

QString ChatTypeConference::name() const
{
	return "ConferenceChat";
}

QString ChatTypeConference::displayName() const
{
	return tr("Conference");
}

QIcon ChatTypeConference::icon() const
{
	return IconsManager::instance()->loadIcon("ManageModules");
}
