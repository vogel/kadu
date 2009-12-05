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

#define PROPERTY_DEC(type, fieldName) \
	type fieldName;
#define PROPERTY_DEF(type, getMethodName, setMethodName, fieldName) \
	type getMethodName() { ensureLoaded(); return fieldName; } \
	void setMethodName(type value) { ensureLoaded(); fieldName = value; }

/**
 * @addtogroup Storage
 * @{
 */
/**
 * @class StorableObject
 * @author Rafal 'Vogel' Malinowski
 * @short Object that can load itself from XML file and store data there.
 *
 * Any class that derives from StorableObject can aasily store and load objects
 * from XML configuration file. File and XML node that contains data about
 * given object is stored using @link<StoragePoint> StoragePoint @endlink
 * object stored in private Storage field.
 *
 * Object can be in one of three @link<StorableObject::StorableObjectState states
 * @endlink that controls behavior of loading and storing.
 *
 * Value of @link<StoragePoint> StoragePoint @endlink can be given to object
 * from outside (so object can load/store to arbitrary node) using @link<StorableObject::setStorage>
 * setStorage @endlink method. When storage point is not set and an attemp
 * to load/store is done, object will create its own storage point using two
 * abstract methods: @link<StorableObject::storageParent> storageParent @endlink
 * and @link<StorableObject::storageNodeName> storageNodeName @endlink.
 */
class KADUAPI StorableObject
{
public:
	/**
	 * @enum StorableObjectState
	 * @author Rafal 'Vogel' Malinowski
	 *
	 * This enum controls how the object behaves when loading/storing data.
	 */
	enum StorableObjectState
	{
		/**
		 * Object is treated as 'new' - one, that has never been stored before.
		 * Such object can not be loaded, @link<ensureLoaded> ensureLoaded @endlink
		 * will do nothing on such object.
		 */
		StateNew,
		/**
		 * Object is treated as 'not loaded' - one, that has not been loaded but is
		 * stored. Such object will be loaded when @link<ensureLoaded> ensureLoaded @endlink
		 * is called.
		 */
		StateNotLoaded,
		/**
		 * Object is treated as 'loaded'. Method @link<ensureLoaded> ensureLoaded @endlink
		 * will have no effect on that object.
		 */
		StateLoaded
	};

private:
	StoragePoint *Storage;
	StorableObjectState State;
	QMap<QString, ModuleData *> ModulesData;

protected:
	virtual StoragePoint * createStoragePoint();

	virtual void load();

public:
	StorableObject();

	virtual StorableObject * storageParent() = 0;
	virtual QString storageNodeName() = 0;

	StoragePoint * storage();

	virtual void store() = 0;

	StorableObjectState state() { return State; }
	void setState(StorableObjectState state) { State = state; }

	void ensureLoaded();
	void removeFromStorage();

	void setStorage(StoragePoint *storage);
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

	void storeValue(const QString &name, const QVariant value);
	void storeAttribute(const QString &name, const QVariant value);

	void removeValue(const QString &name);
	void removeAttribute(const QString &name);

	void storeModuleData();

};

/**
 * @}
 */

#endif // STORABLE_OBJECT_H
