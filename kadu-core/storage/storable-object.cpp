/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QtAlgorithms>

#include "storage/module-data.h"

#include "storable-object.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Contructs object with StateNew state and null storage point.
 *
 * Constructs object with @link<StorableObject::StateNew state @endlink and null
 * (invalid) @link<StorableObject::storage storage point @endlink.
 */
StorableObject::StorableObject() :
		Destroying(false), State(StateNew)
{
}

StorableObject::~StorableObject()
{
	Destroying = true;

	qDeleteAll(ModulesStorableData);
	// TODO: 0.10.0, memory leak
// 	foreach (void *moduleData, ModulesData)
// 		delete moduleData;
}

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
QSharedPointer<StoragePoint> StorableObject::createStoragePoint()
{
	if (storageNodeName().isEmpty())
		return QSharedPointer<StoragePoint>();

	StorableObject *parent = storageParent();
	if (!parent)
		return QSharedPointer<StoragePoint>(new StoragePoint(xml_config_file, xml_config_file->getNode(storageNodeName())));

	QSharedPointer<StoragePoint> parentStoragePoint(storageParent()->storage());
	if (!parentStoragePoint)
		return QSharedPointer<StoragePoint>();

	QDomElement node = parentStoragePoint->storage()->getNode(parentStoragePoint->point(), storageNodeName());
	return QSharedPointer<StoragePoint>(new StoragePoint(parentStoragePoint->storage(), node));
}

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
void StorableObject::setStorage(const QSharedPointer<StoragePoint> &storage)
{
	State = StateNotLoaded;
	Storage = storage;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns true if storage point is valid.
 * @return true if storage point is valid
 *
 * Storage is valid when it is not NULL and points to a real XML storage file.
 */
bool StorableObject::isValidStorage()
{
	return storage() && storage()->storage();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns storage point for this object.
 * @return storage point for this object
 *
 * Returns storage point for this object. If the storage point has not been specified
 * yet, it calls @link<createStoragePoint> createStoragePoint @endlink to create one.
 */
const QSharedPointer<StoragePoint> & StorableObject::storage()
{
	if (!Storage)
		Storage = createStoragePoint();

	return Storage;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores object data in XML node.
 *
 * Stores all module data object by calling store method on these objects.
 * Reimplementations of this method should store all needed object data
 * using storeValue and storeAttribute methods and should call super class
 * method.
 */
void StorableObject::store()
{
	ensureLoaded();

	foreach (ModuleData *moduleData, ModulesStorableData)
		moduleData->store();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Determines if object is worth to be stored.
 * @return true if object should be stored, defaults to true
 * @todo this method is used only in managers, in 0.10 it should be used in every place
 *
 * If object is incomplete or invalid this method should return false
 * so it will not be stored in persistent storage.
 */
bool StorableObject::shouldStore()
{
	return true;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads data from storage point. Sets state to StateLoaded.
 *
 * This is base implementation of load method, that is called by ensureLoaded method.
 * This version only sets state to StateLoaded. This method must be overridden in every
 * derivered class that has real data to read. This method must be called by every
 * reimplementation, if possible at beginning.
 */
void StorableObject::load()
{
	State = StateLoaded;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Ensures that this object data has been loaded.
 *
 * This method loads data (by calling load method) only when current state of object
 * is StateNotLoaded. New object and already loaded object are not loaded twice.
 * Load method is responsible for changing the state to StateLoaded.
 */
void StorableObject::ensureLoaded()
{
	if (StateNotLoaded == State)
		load();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores or removes data from storage, depends on shouldStore result.
 *
 * If shouldStore method returns true this method stores object in storage file.
 * Else, object is removed from storage.
 */
void StorableObject::ensureStored()
{
	if (shouldStore())
		store();
	else
		removeFromStorage();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removed object from storage.
 *
 * Removes current object from storage (it will not be possible to load it anymore).
 * It is still possible to store this object in other place by using setStorage
 * method.
 */
void StorableObject::removeFromStorage()
{
	if (!Storage)
		return;

	Storage->point().parentNode().removeChild(Storage->point());
	Storage.clear();
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes non-storable module data from object.
 * @param module name of module data to be removed
 *
 * Removed module data for given key. Caller is responsible for freeing
 * memory used by this module data.
 */
void StorableObject::removeModuleData(const QString& module)
{
	if (ModulesData.contains(module))
		ModulesData.remove(module);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Called when ModuleData is destroyed, to remove it from this object.
 * @param moduleName name of ModuleData
 * @param moduleData destroyed object
 *
 * Method is called by ModuleData class in its destructor, so it can be removed
 * from this object.
 */
void StorableObject::moduleDataDestroyed(const QString &moduleName, ModuleData *moduleData)
{
	if (Destroying)
		return;

	if (ModulesData.contains(moduleName) && ModulesData.value(moduleName) == moduleData)
		ModulesData.remove(moduleName);

	if (ModulesStorableData.contains(moduleName) && ModulesStorableData.value(moduleName) == moduleData)
	{
		moduleData->store();
		ModulesStorableData.remove(moduleName);
	}
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
void StorableObject::storeValue(const QString &name, const QVariant value)
{
	Storage->storage()->createTextNode(Storage->point(), name, value.toString());
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores value into XML node (as an attribute).
 * @param name name of attribute that will store this value
 * @param value value to be stored
 *
 * Stores value into XML node as a attribute 'name' with value 'value'
 * (value is converted to QString before storing).
 */
void StorableObject::storeAttribute(const QString &name, const QVariant value)
{
	Storage->point().setAttribute(name, value.toString());
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes value (a subnode) from XML node.
 * @param name name of subnode that will be removed
 *
 * Removes subnode 'name' from XML storage file.
 */
void StorableObject::removeValue(const QString& name)
{
	Storage->storage()->removeNode(Storage->point(), name);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes value (an attribute) from XML node.
 * @param name name of attribute that will be removed
 *
 * Removes attribute 'name' from XML storage file.
 */
void StorableObject::removeAttribute(const QString& name)
{
	Storage->point().removeAttribute(name);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates storage point object for given module data.
 * @param module name of module data
 * @param create if true this method can create new nodes
 * @return storage point object for given module data
 *
 * Creates storage point for given module data. If XML node is not present
 * and create parameter is false this method will return NULL. Else it will
 * return storage point that points for right XML node (even if creating new
 * XML node is needed).
 *
 * Node is named ModuleData with attribute name with value from module parameter.
 */
QSharedPointer<StoragePoint> StorableObject::storagePointForModuleData(const QString &module, bool create)
{
	QSharedPointer<StoragePoint> parent(storage());
	if (!parent || !parent->storage())
		return QSharedPointer<StoragePoint>();

	QDomElement moduleDataNode = parent->storage()->getNamedNode(parent->point(), "ModuleData",
			module, create ? XmlConfigFile::ModeGet : XmlConfigFile::ModeFind);
	return moduleDataNode.isNull()
			? QSharedPointer<StoragePoint>()
			: QSharedPointer<StoragePoint>(new StoragePoint(parent->storage(), moduleDataNode));
}
