/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <memory>

#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "exports.h"

#define PROPERTY_DEC(type, fieldName) \
	type fieldName;
#define PROPERTY_DEF(type, getMethodName, setMethodName, fieldName) \
	type getMethodName() { ensureLoaded(); return fieldName; } \
	void setMethodName(type value) { ensureLoaded(); fieldName = value; }

class CustomProperties;

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
 * Custom properties can be stored in @link CustomProperties @endlink class that is accessible by
 * @link customProperties() @endlink getter. Storage mechanism for these values is described in
 * @link CustomProperties @endlink documentation.
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
	std::shared_ptr<StoragePoint> Storage;
	StorableObjectState State;
	CustomProperties *Properties;

protected:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Creates default storage point for object.
	 *
	 * Constructs storage point: XML node that is child of storage point of object
	 * returned by @link<StorableObject::storageParent storageParent @endlink method.
	 * Node name is given by @link<StorableObject::storageNodeName @endlink method.
	 *
	 * If @link<StorableObject::storageNodeName @endlink returns invalid node name
	 * (empty string) or @link<StorableObject::storageParent storageParent @endlink
	 * returns object that has invalid storage point, this method returns invalid
	 * storage point.
	 *
	 * If parent is NULL this method will return storage point that is child of
	 * root node of XML configuration file.
	 */
	virtual std::shared_ptr<StoragePoint> createStoragePoint();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads data from storage point. Sets state to StateLoaded.
	 *
	 * This is base implementation of load method, that is called by ensureLoaded method.
	 * This version only sets state to StateLoaded and loads all custom properties. This
	 * method must be overridden in every derivered class that has real data to read. This
	 * method must be called by every reimplementation, if possible at beginning.
	 */
	virtual void load();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores object data in XML node.
	 *
	 * Stores object data in XML node. Also all storable custom properties are stored.
	 */
	virtual void store();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Determines if object is worth to be stored.
	 * @return true if object should be stored, defaults to true
	 *
	 * If object is incomplete, invalid or unneeded in storage, this method should return false
	 * so it will not be stored in persistent storage. It is a good practice to reimplement this
	 * method. Value returned by super class should be always considered.
	 *
	 * Default implementation returns true.
	 */
	virtual bool shouldStore();

	virtual void loaded() { };

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Contructs object with StateNew state and null storage point.
	 *
	 * Constructs object with @link<StorableObject::StateNew state @endlink and null
	 * (invalid) @link<StorableObject::storage storage point @endlink.
	 */
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

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns storage point for this object.
	 * @return storage point for this object
	 *
	 * Returns storage point for this object. If the storage point has not been specified
	 * yet, it calls @link<createStoragePoint> createStoragePoint @endlink to create one.
	 */
	const std::shared_ptr<StoragePoint> & storage();

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

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Ensures that this object data has been loaded.
	 *
	 * This method loads data (by calling load method) only when current state of object
	 * is StateNotLoaded. New object and already loaded object are not loaded twice.
	 * Load method is responsible for changing the state to StateLoaded.
	 */
	void ensureLoaded();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores or removes data from storage, depends on shouldStore result.
	 *
	 * If shouldStore method returns true this method stores object in storage file.
	 * Else, object is removed from storage.
	 */
	void ensureStored();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removed object from storage.
	 *
	 * Removes current object from storage (it will not be possible to load it anymore).
	 * It is still possible to store this object in other place by using setStorage
	 * method.
	 */
	void removeFromStorage();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sets arbitrary storage for this object. Sets state to StateNotLoaded.
	 * @param storage new storage point
	 *
	 * This method allows you to set arbitrary storage point. Use that method when place
	 * of data storage is known and the data needs to be loaded. This method changes
	 * state of object to StateNotLoaded, so it will be loaded after executing ensureLoaded
	 * method.
	 */
	void setStorage(const std::shared_ptr<StoragePoint> &storage);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns true if storage point is valid.
	 * @return true if storage point is valid
	 *
	 * Storage is valid when it is not NULL and points to a real XML storage file.
	 */
	bool isValidStorage();

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
		return Storage->loadAttribute<T>(name);
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
		return Storage->loadValue<T>(name);
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
		return Storage->hasValue(name);
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
		return Storage->loadAttribute(name, def);
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
		return Storage->loadValue(name, def);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores value into XML node (as a subnode).
	 * @param name name of subnode that will store this value
	 * @param value value to be stored
	 *
	 * Stores value into XML node as a subnode 'name' with value 'value'
	 * (value is converted to QString before storing).
	 */
	void storeValue(const QString &name, const QVariant value);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores value into XML node (as an attribute).
	 * @param name name of attribute that will store this value
	 * @param value value to be stored
	 *
	 * Stores value into XML node as a attribute 'name' with value 'value'
	 * (value is converted to QString before storing).
	 */
	void storeAttribute(const QString &name, const QVariant value);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes value (a subnode) from XML node.
	 * @param name name of subnode that will be removed
	 *
	 * Removes subnode 'name' from XML storage file.
	 */
	void removeValue(const QString &name);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes value (an attribute) from XML node.
	 * @param name name of attribute that will be removed
	 *
	 * Removes attribute 'name' from XML storage file.
	 */
	void removeAttribute(const QString &name);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Return CustomProperties instance for this object.
	 * @return CustomProperties instance for this objects
	 *
	 * Returned object must not be deleted. This object has full ownership over it.
	 */
	CustomProperties * customProperties() const;

};

/**
 * @}
 */

#endif // STORABLE_OBJECT_H
