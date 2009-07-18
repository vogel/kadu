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

#include "configuration/xml-configuration-file.h"

#include "modules/module-data.h"
#include "storage-point.h"

#include "exports.h"

class KADUAPI StorableObject
{
	StorableObject *Parent;
	QString NodeName;
	StoragePoint *Storage;
	bool Loaded;
	QMap<QString, ModuleData *> ModulesData;

protected:
	virtual StoragePoint * createStoragePoint();

public:
	explicit StorableObject(bool loaded = false);
	StorableObject(StoragePoint *storage, bool loaded = false);
	StorableObject(const QString &nodeName, StorableObject *parent, bool loaded = false);

	StorableObject * parent() { return Parent; }
	QString nodeName() { return NodeName; }

	StoragePoint * storage();

	virtual void load();
	virtual void store() = 0;
	bool isLoaded() { return Loaded; }
	void setLoaded(bool loaded) { Loaded = loaded; }
	void ensureLoaded();
	void removeFromStorage();

	void setStorage(StoragePoint *storage) { Storage = storage; }
	bool isValidStorage() { return storage() && storage()->storage(); }
	StoragePoint * storagePointForModuleData(const QString &module, bool create = false);

template<class T>
	T loadAttribute(const QString &name) const
	{
		QVariant value = Storage->point().attribute(name);
		return value.value<T>();
	}

template<class T>
	T loadValue(const QString &name) const
	{
		QVariant value;

		if (Storage->storage()->hasNode(Storage->point(), name))
			value = Storage->storage()->getTextNode(Storage->point(), name);

		return value.value<T>();
	}

template<class T>
	T loadAttribute(const QString &name, T def) const
	{
		if (Storage->point().hasAttribute(name))
		{
			QVariant value = Storage->point().attribute(name);
			return value.value<T>();
		}

		return def;
	}

template<class T>
	T loadValue(const QString &name, T def) const
	{
		if (Storage->storage()->hasNode(Storage->point(), name))
		{
			QVariant value = Storage->storage()->getTextNode(Storage->point(), name);
			return value.value<T>();
		}

		return def;
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

	void storeValue(const QString &name, const QVariant value, bool attribute = false);
	void removeValue(const QString &name, bool attribute = false);

	void storeModuleData();

};

#endif // STORABLE_OBJECT_H
