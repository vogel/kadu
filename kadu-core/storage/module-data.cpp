/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "module-data.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Contructs object with StateNew state, null storage point and given parent StorableObject object.
 * @param storageParent parent o
 *
 * Constructs object with @link<StorableObject::StateNew state @endlink and null
 * (invalid) @link<StorableObject::storage storage point @endlink.
 *
 * This object will be stored in node ModuleData under node defined by storageParent object.
 * ModuleData nodes are distinguished by 'name' attribute (that is set by @link<storageNodeName>
 * storageNodeName @endlink method.
 */
ModuleData::ModuleData(StorableObject *storageParent)
		: CurrentStorageParent(storageParent)
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns object that holds parent storage point for this object.
 * @return node name of XML storage of this object
 *
 * This implementation returns object set in contructor.
 */
StorableObject * ModuleData::storageParent()
{
	return CurrentStorageParent;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns value of 'name' attribute that will be added to XML node.
 * @return value of 'name' attribute that will be added to XML node
 *
 * Returns value of 'name' attribute that will be added to XML node.
 */
QString ModuleData::storageNodeName()
{
	return QLatin1String("ModuleData");
}
