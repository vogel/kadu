/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc.h"
#include "configuration_aware_object.h"
#include <QList>

QList<ConfigurationAwareObject *> ConfigurationAwareObject::objects;

void ConfigurationAwareObject::registerObject(ConfigurationAwareObject *object)
{
	objects.append(object);
}

void ConfigurationAwareObject::unregisterObject(ConfigurationAwareObject *object)
{
	objects.remove(object);
}

void ConfigurationAwareObject::notifyAll()
{
	FOREACH(object, objects)
		(*object)->configurationUpdated();
}

ConfigurationAwareObject::ConfigurationAwareObject()
{
	registerObject(this);
}

ConfigurationAwareObject::~ConfigurationAwareObject()
{
	unregisterObject(this);
}

