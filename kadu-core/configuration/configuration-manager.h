/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIGURATION_MANAGER
#define CONFIGURATION_MANAGER

#include <QtCore/QList>
#include <QtCore/QUuid>

#include "exports.h"

class StorableObject;
class ToolbarConfigurationManager;

class KADUAPI ConfigurationManager
{
	static ConfigurationManager *Instance;

	QUuid Uuid;
	QList<StorableObject *> RegisteredStorableObjects;

	ToolbarConfigurationManager *ToolbarConfiguration;

	ConfigurationManager();
	~ConfigurationManager();

	void importConfiguration();

public:
	static ConfigurationManager * instance();

	void load();
	void store();
	void flush();

	const QUuid & uuid() const { return Uuid; }

	void registerStorableObject(StorableObject *object);
	void unregisterStorableObject(StorableObject *object);

	ToolbarConfigurationManager * toolbarConfigurationManager() { return ToolbarConfiguration; }

};

#endif // CONFIGURATION_MANAGER
