/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIGURATION_MANAGER
#define CONFIGURATION_MANAGER

#include <QtCore/QList>
#include <QtCore/QUuid>

#include "exports.h"

class StorableObject;

class KADUAPI ConfigurationManager
{
	static ConfigurationManager *Instance;

	QUuid Uuid;
	QList<StorableObject *> RegisteredStorableObjects;

	ConfigurationManager();

	void importConfiguration();
	void copyOldContactsToImport();
	void importOldContact(); // TODO: remove after 0.6.6 release
	void importContactsIntoBuddies(); // TODO: remove after 0.6.6 release
	void importContactAccountDatasIntoContacts(); // TODO: remove after 0.6.6 release

public:
	static ConfigurationManager * instance();

	void load();
	void store();
	void flush();

	QUuid uuid() { return Uuid; }

	void registerStorableObject(StorableObject *object);
	void unregisterStorableObject(StorableObject *object);

};

#endif // CONFIGURATION_MANAGER
