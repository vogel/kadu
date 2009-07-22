/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "notify-event.h"

NotifyEvent::NotifyEvent(const QString &name, NotifyEvent::CallbackRequirement isCallbackRequired, const char *description) :
		Name(name), IsCallbackRequired(isCallbackRequired), Description(description)
{
	int index = Name.indexOf("/");
	Category = (index > 0) ? Name.mid(0, index) : QString();
}

bool NotifyEvent::operator == (const NotifyEvent &compare)
{
	return Name == compare.Name;
}
