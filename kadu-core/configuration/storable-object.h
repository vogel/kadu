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

#include "modules/module-data.h"
#include "storage-point.h"

class StorableObject
{
	StorableObject *Parent;
	QString NodeName;
	StoragePoint *Storage;
	QMap<QString, ModuleData *> ModulesData;

protected:
	virtual StoragePoint * createStoragePoint();

public:
	StorableObject();
	StorableObject(const QString &nodeName, StorableObject *parent);

	StorableObject * parent() { return Parent; }
	QString nodeName() { return NodeName; }

	StoragePoint * storage();

	virtual void store() = 0;
	void removeFromStorage();

	void setStorage(StoragePoint *storage) { Storage = storage; }
	bool isValidStorage() { return storage() && storage()->storage(); }
	StoragePoint * storagePointForModuleData(const QString &module, bool create = false);

template<class T>
	T loadValue(const QString &name) const
	{
		QVariant value;

		if (Storage->storage()->hasNode(Storage->point(), name))
			value = Storage->storage()->getTextNode(Storage->point(), name);

		return value.value<T>();
	}
	// TODO: 0.6.6 - check create and cache implementation
template<class T>
	T * moduleData(bool create = false, bool cache = false)
	{
		T *result = 0;
		if (!cache)
		{
			result = new T(storagePointForModuleData(T::key(), create));
			result->loadFromStorage();
		}
		else
		{
			if (!ModulesData.contains(T::key()))
			{
				result = new T(storagePointForModuleData(T::key(), create));
				ModulesData[T::key()] = result;
				result->loadFromStorage();
			}
			else
				result = dynamic_cast<T *>(ModulesData[T::key()]);

		}
		return result;
	}

	void storeValue(const QString &name, const QVariant value);
	void storeModuleData();

};

#endif // STORABLE_OBJECT_H
