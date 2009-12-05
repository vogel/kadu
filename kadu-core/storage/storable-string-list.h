/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORABLE_STRING_LIST_H
#define STORABLE_STRING_LIST_H

#include <QtCore/QStringList>

#include "storage/storable-object.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class StorableStringList
 * @author Rafal 'Vogel' Malinowski
 * @short QStringList that can load itself from XML file and store data there.
 *
 * This class is QStringList extended to have the possibility of loading and storing
 * data from/to XML configuration file.
 */
class StorableStringList : protected QStringList, public StorableObject
{

protected:
	virtual void load();

public:
	StorableStringList();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns name of subnodes that stores strings.
	 * @return name of subnodes that stores strings
	 *
	 * All strings from list are stored into separate subnodes.
	 * This method determines names of these subnodes.
	 */
	virtual QString storageItemNodeName() = 0;
	virtual void store();

	const QStringList content() const;

};

/**
 * @}
 */

#endif // STORABLE_STRING_LIST_H
