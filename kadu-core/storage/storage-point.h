/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORAGE_POINT_H
#define STORAGE_POINT_H

#include <QtXml/QDomElement>

/**
 * @ingroup Storage
 * @{
 */

class XmlConfigFile;

/**
 * @class StoragePoint
 * @author Rafal 'Vogel' Malinowski
 * @short Unique place that can store any data.
 *
 * StoragePoint provides unique place that can store any data
 * Storage point consists of two parts:
 * <ul>
 *   <li>storage - pointer to XML file that contains persistent data</li>
 *   <li>point - DOM node that is parent of all nodes this object uses for storage</li>
 * </ul>
 *
 * Storage points are created in @link StorableObject class in @link createStoragePoint
 * method. You generally should not create instances of StoragePoint outside these
 * classes. Consider derivering from them instead.
 */
class StoragePoint
{
	XmlConfigFile *Storage;
	QDomElement Point;

public:
	StoragePoint(XmlConfigFile *storage, QDomElement point);

	XmlConfigFile * storage();
	QDomElement point();

};

/**
 * @}
 */

#endif // STORAGE_POINT_H
