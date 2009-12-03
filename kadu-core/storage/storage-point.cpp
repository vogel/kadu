/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "storage/storage-point.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Constructs StoragePoint.
 * @arg storage pointer to XML storage file
 * @arg point DOM Node - parent of all data stored by this class
 *
 * Constructs StoragePoint from given XML file and DOM node.
 */
StoragePoint::StoragePoint(XmlConfigFile *storage, QDomElement point)
		: Storage(storage), Point(point)
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns pointer to current XML storage file.
 *
 * Returns pointer to current XML storage file.
 */
XmlConfigFile * StoragePoint::storage()
{
	return Storage;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns current parent DOM Node for this object.
 *
 * Returns current parent DOM Node for this object.
 */
QDomElement StoragePoint::point()
{
	return Point;
}
