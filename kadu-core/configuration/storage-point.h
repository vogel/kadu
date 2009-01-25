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

class XmlConfigFile;

class StoragePoint
{
	XmlConfigFile *Storage;
	QDomElement Point;

public:
	StoragePoint(XmlConfigFile *storage, QDomElement point)
		: Storage(storage), Point(point) {}

	XmlConfigFile * storage() { return Storage; }
	QDomElement point() { return Point; }

};

#endif // STORAGE_POINT_H
