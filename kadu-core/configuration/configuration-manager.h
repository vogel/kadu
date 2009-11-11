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

#include "exports.h"

class StorableObject;

class KADUAPI ConfigurationManager
{
	QList<StorableObject *> RegisteredStorableObjects;

	void importConfiguration();
	void copyOldContactsToImport();
	void importOldContact(); // TODO: remove after 0.6.6 release
	void importContactsIntoBuddies(); // TODO: remove after 0.6.6 release
	void importContactAccountDatasIntoContacts(); // TODO: remove after 0.6.6 release

public:
	ConfigurationManager();

	void load();
	void store();
	void flush();

	void registerStorableObject(StorableObject *object);
	void unregisterStorableObject(StorableObject *object);

};

#endif // CONFIGURATION_MANAGER
