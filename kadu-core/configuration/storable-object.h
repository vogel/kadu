/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORABLE_OBJECT_H
#define STORABLE_OBJECT_H

#include <QtCore/QVariant>

#include "xml_config_file.h"

#include "storage-point.h"

class StorableObject
{
	StorableObject *Parent;
	QString NodeName;
	StoragePoint *Storage;

protected:
	virtual StoragePoint * createStoragePoint();

public:
	StorableObject();
	StorableObject(const QString &nodeName, StorableObject *parent);

	StorableObject * parent() { return Parent; }
	QString nodeName() { return NodeName; }

	StoragePoint * storage();
	void removeFromStorage();

	void setStorage(StoragePoint *storage) { Storage = storage; }
	bool isValidStorage() { return storage() && storage()->storage(); }

template<class T>
	T loadValue(const QString &name) const
	{
		QVariant value;

		if (Storage->storage()->hasNode(Storage->point(), name))
			value = Storage->storage()->getTextNode(Storage->point(), name);

		return value.value<T>();
	}

	void storeValue(const QString &name, const QVariant value);

};

#endif // STORABLE_OBJECT_H
