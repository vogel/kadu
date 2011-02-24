/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STORABLE_OBJECT_H
#define STORABLE_OBJECT_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "exports.h"

#define PROPERTY_DEC(type, fieldName) \
	type fieldName;
#define PROPERTY_DEF(type, getMethodName, setMethodName, fieldName) \
	type getMethodName() { ensureLoaded(); return fieldName; } \
	void setMethodName(type value) { ensureLoaded(); fieldName = value; }

class ModuleData;

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class StorableObject
 * @author Rafal 'Vogel' Malinowski
 * @short Object that can load itself from XML file and store data there.
 *
 * Any class that derives from StorableObject can easily store and load objects
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
 *
 * Storing example:
 *
 * <pre>
 * storeAttribute("id", 15);
 * storeValue("ApplicationName", "Kadu");
 * storeValue("ApplicationVersion", "0.6.6-master");
 * </pre>
 *
 * when storageNodeName is defined as:
 *
 * <pre>
 * return QLatin1String("Application");
 * </pre>
 *
 * will result in following XML structure:
 *
 * <pre>
 * &lt;Application id="15"&gt;
 *   &lt;ApplicationName&gt;Kadu&lt;ApplicationName&gt;
 *   &lt;ApplicationVersion&gt;0.6.6&lt;ApplicationVersion&gt;
 * &lt;/Application&gt;
 * </pre>
 *
 * To load these values use:
 *
 * <pre>
 * Id = loadAttribute&lt;int&gt;("id");
 * Name = loadValue&lt;QString&gt;("ApplicationName");
 * Version = loadValue&lt;QString&gt;("ApplicationVersion");
 * </pre>
 *
 * Every plugin can attach any data to any StorableObject by using @link moduleData @endlink
 * system. It allows to create named subnodes of arbitrary types under main XML node
 * of StorableObject. All module data object are stored as ModuleData subnodes and are
 * identified by 'name' attribute, that has to be unique per ModuleData subtype.
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
	bool Destroying;
	QSharedPointer<StoragePoint> Storage;
	StorableObjectState State;
	QMap<QString, ModuleData *> ModulesStorableData;
	QMap<QString, void *> ModulesData;

	friend class ModuleData;
	void moduleDataDestroyed(const QString &moduleName, ModuleData *moduleData);

protected:
	virtual QSharedPointer<StoragePoint> createStoragePoint();

	virtual void load();

public:
	StorableObject();
	virtual ~StorableObject();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns object that holds parent storage point for this object.
	 *
	 * Reimplementations of this method in derivered classes should return object
	 * that holds parent storage point for this object. If NULL is returned, this
	 * object will be stored directly below root XML node of storage file. If value
	 * is not NULL, this object will be stored below XML node of parent object.
	 */
	virtual StorableObject * storageParent() = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns node name of XML storage of this object.
	 * @return node name of XML storage of this object
	 *
	 * Reimplementations of this method in derivered classes should return name
	 * of XML node that holds this object data.
	 */
	virtual QString storageNodeName() = 0;

	const QSharedPointer<StoragePoint> & storage();

	virtual void store();
	virtual bool shouldStore();

	virtual void loaded() { };
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns current object state.
	 * @return current object state
	 *
	 * Return current object state.
	 */
	StorableObjectState state() { return State; }

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sets new state of object.
	 * @param state object's new state
	 *
	 * Sets new state of object.
	 */
	void setState(StorableObjectState state) { State = state; }

	void ensureLoaded();
	void ensureStored();
	void removeFromStorage();

	void setStorage(const QSharedPointer<StoragePoint> &storage);
	bool isValidStorage();
	QSharedPointer<StoragePoint> storagePointForModuleData(const QString &module, bool create = false);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as an attribute).
	 * @param T type of returned value
	 * @param name name of attribute that will be loaded
	 * @return value of XML attribute
	 *
	 * Loads value from XML node as an attribute 'name' with type T.
	 */
template<class T>
	T loadAttribute(const QString &name) const
	{
		QVariant value = Storage->point().attribute(name);
		return value.value<T>();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as subnode).
	 * @param T type of returned value
	 * @param name name of subnode that will be loaded
	 * @return value of XML subnode
	 *
	 * Loads value from XML node as subnode 'name' with type T.
	 */
template<class T>
	T loadValue(const QString &name) const
	{
		QVariant value;

		if (Storage->storage()->hasNode(Storage->point(), name))
			value = Storage->storage()->getTextNode(Storage->point(), name);

		return value.value<T>();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Check if value is available in XML node (as subnode).
	 * @param name name of subnode that will be checked
	 * @return true, if subnode is available
	 *
	 * Check if value is available in XML node (as subnode).
	 */
	bool hasValue(const QString &name) const
	{
		return Storage->storage()->hasNode(Storage->point(), name);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as an attribute).
	 * @param T type of returned value
	 * @param name name of attribute that will be loaded
	 * @param def default value, returned when attribute non present
	 * @return value of XML attribute
	 *
	 * Loads value from XML node as an attribute 'name' with type T.
	 * If attribute is non present this method will return value of def.
	 */
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

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads value from XML node (as subnode).
	 * @param T type of returned value
	 * @param name name of subnode that will be loaded
	 * @param def default value, returned when subnode non present
	 * @return value of XML subnode
	 *
	 * Loads value from XML node as subnode 'name' with type T.
	 * If subnode is non present this method will return value of def.
	 */
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

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads storable ModuleData data from XML node (as subnode).
	 * @param T type of returned value (must be class that inherits from @link ModuleData @endlink)
	 * @param module name of module to be loaded
	 * @param qobjectParent QObject parent of new object, it will be responsible for deleting data on module unloading
	 * @param create when true this method can create new ModuleData (if non present)
	 * @return value of XML subnode, as an object
	 *
	 * Loads object from XML subnode 'modules' with type T. If subnode is non present
	 * and create is false this method will return NULL value, else it will at least
	 * create new object with default values.
	 */
template<class T>
	T * moduleStorableData(const QString &module, QObject *qobjectParent, bool create)
	{
		if (ModulesStorableData.contains(module))
			return qobject_cast<T *>(ModulesStorableData[module]);

		QSharedPointer<StoragePoint> storagePoint(storagePointForModuleData(module, create));
		if (!storagePoint)
			return 0;

		T *result = new T(module, this, qobjectParent);
		result->setState(StateNew);
		result->setStorage(storagePoint);
		ModulesStorableData.insert(module, result);
		return result;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns non-storable module data for object.
	 * @param T type of returned value (any class)
	 * @param module name of module to be loaded
	 * @param create when true this method can create new ModuleData (if non present)
	 * @return object of type T assigned with this storable object
	 *
	 * Returns object of type T with name module assigned with this obejct. If no
	 * gived object is present and create is set to true, new object is created,
	 * assigned and returned.
	 */
template<class T>
	T * moduleData(const QString &module, bool create = false)
	{
		if (ModulesData.contains(module))
			return static_cast<T *>(ModulesData[module]);

		if (!create)
			return 0;

		T *result = new T();
		ModulesData.insert(module, result);
		return result;
	}

	void removeModuleData(const QString &module);

	void storeValue(const QString &name, const QVariant value);
	void storeAttribute(const QString &name, const QVariant value);

	void removeValue(const QString &name);
	void removeAttribute(const QString &name);

};

/**
 * @}
 */

#endif // STORABLE_OBJECT_H
