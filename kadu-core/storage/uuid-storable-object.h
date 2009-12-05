/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef UUID_STORABLE_OBJECT_H
#define UUID_STORABLE_OBJECT_H

#include <QtCore/QUuid>

#include "storable-object.h"

#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */
/**
 * @class UuidStorableObject
 * @author Rafal 'Vogel' Malinowski
 * @short Object that can load itself from XML file and store data there. Object is identified by UUID.
 *
 * Many objects of this class can be stored under one master XML node in storage.
 * Each object is identified by unique identifier (called UUID) that never changes
 * after object is created.
 */
class KADUAPI UuidStorableObject : public StorableObject
{

protected:
	virtual StoragePoint * createStoragePoint();

public:
	UuidStorableObject();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns unique identifier of this object.
	 * @return unique identifier of this object
	 *
	 * This method returns object of type QUuid that represens unique ID for this object.
	 * Each derivered class should ovveride that method to return really unique ID.
	 */
	virtual QUuid uuid() const = 0;

};

/**
 * @}
 */

#endif // UUID_STORABLE_OBJECT_H
