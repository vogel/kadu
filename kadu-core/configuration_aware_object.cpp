/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "configuration_aware_object.h"

KADU_AWARE_CLASS(ConfigurationAwareObject)

void ConfigurationAwareObject::notifyAll()
{
	foreach (ConfigurationAwareObject *object, Objects)
		object->configurationUpdated();
}
