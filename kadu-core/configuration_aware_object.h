/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIGURATION_AWARE_OBJECT
#define CONFIGURATION_AWARE_OBJECT

#include <qvaluelist.h>

class ConfigurationAwareObject
{
	static QValueList<ConfigurationAwareObject *> objects;

	static void registerObject(ConfigurationAwareObject *object);
	static void unregisterObject(ConfigurationAwareObject *object);

public:
	static void notifyAll();

	ConfigurationAwareObject();
	virtual ~ConfigurationAwareObject();

	virtual void configurationUpdated() = 0;
};

#endif // CONFIGURATION_AWARE_OBJECT
