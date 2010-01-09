/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MANAGER_H
#define MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/configuration-manager.h"
#include "storage/storable-object.h"

#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class Manager
 * @author Rafal 'Vogel' Malinowski
 * @param Item class type of manager items, must be derivered from UuidStorableObject and DetailsHolder.
 * @short Object that manages instances of given Item type (including storing and loading from XML file).
 *
 * This object manages items of Item type - it allows adding, removing, searching by uuid,
 * index, loading and storing from XML configuration file.
 *
 * Only object with details set are (by default) accessible by this manager. Objects without
 * details can only by accesses by @link byUuid @endlink and @link allItems @endlink
 * methods. Object with details are called "registered" objects.
 *
 * As a template, this class cannot be QObject and cannot emit signals. Each derivered class
 * that has to emits signals must also derive from QObject class and reimplement abstract
 * functions: @link itemAboutToBeRegistered @endlink, @link itemRegistered @endlink,
 * @link itemAboutToBeUnregistered @endlink, @link itemUnregisterd @endlink to emit signals.
 *
 * To manager objects not derivered from @link DetailsHolder @endlink class use
 * @link SimpleManager @endlink template class.
 *
 * Class Item must implement Item loadFromStorage(StoragePoint *) static method.
 * Class Item must have static field Item null that represents unique NULL value.
 */
template<class Item>
class KADUAPI Manager : public StorableObject
{
	QList<Item> Items;
	QList<Item> ItemsWithDetails;

protected:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Contructs empty, 'not loaded', object.
	 *
	 * Contructs empty, 'not loaded', object. Also registers this object
	 * in @link ConfigurationManager @endlink singleton, so this class
	 * will automatically store itself on each configuration flush request.
	 */
	Manager()
	{
		setState(StateNotLoaded);
		ConfigurationManager::instance()->registerStorableObject(this);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Destroys object.
	 *
	 * Destroys object. Also unregisters this object in @link ConfigurationManager @endlink
	 * singleton.
	 */
	virtual ~Manager()
	{
		ConfigurationManager::instance()->unregisterStorableObject(this);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns storage parent of this object.
	 * @return storage parent of this object
	 *
	 * Returns NULL. This class stores configuration just under root node of
	 * XML configuration file.
	 */
	virtual StorableObject * storageParent()
	{
		return 0;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns node name for items stored in this class.
	 * @return node name for items stored in this class
	 *
	 * Returns node name for items stored in this class.
	 */
	virtual QString storageNodeItemName() = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just before item is added to manager.
	 *
	 * This method is called just before item is added to manager.
	 */
	virtual void itemAboutToBeAdded(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just after item is added to manager.
	 *
	 * This method is called just after item is added to manager.
	 */
	virtual void itemAdded(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just before item is removed from manager.
	 *
	 * This method is called just before item is removed from manager.
	 */
	virtual void itemAboutToBeRemoved(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just after item is removed from manager.
	 *
	 * This method is called just after item is removed from manager.
	 */
	virtual void itemRemoved(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just before item is registered in manager.
	 *
	 * This method is called just before item is registered in manager
	 * (that means item added to manager gets details or item with details
	 * is added).
	 */
	virtual void itemAboutToBeRegistered(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just after item is registered in manager.
	 *
	 * This method is called just after item is registered in manager
	 * (that means item added to manager gets details or item with details
	 * is added).
	 */
	virtual void itemRegistered(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just before item is unregistered in manager.
	 *
	 * This method is called just before item is unregistered in manager
	 * (that means item added to manager loses details or item with details
	 * is removed).
	 */
	virtual void itemAboutToBeUnregisterd(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just after item is unregistered in manager.
	 *
	 * This method is called just after item is unregistered in manager
	 * (that means item added to manager loses details or item with details
	 * is removed).
	 */
	virtual void itemUnregistered(Item item)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Registers item already added to manager.
	 *
	 * This method should be run after details has been set on item that
	 * has already been added to manager. This method has no effect
	 * for items not added to manager or to items that has already been
	 * registered. This method calls @link itemAboutToBeRegistered @endlink
	 * and @link itemRegistered @endlink.
	 */
	void registerItem(Item item)
	{
		if (ItemsWithDetails.contains(item))
			return;
		if (!Items.contains(item))
			return;

		itemAboutToBeRegistered(item);
		ItemsWithDetails.append(item);
		itemRegistered(item);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Unegisters item already added to manager.
	 *
	 * This method should be run after details has been removed from item that
	 * has already been added to manager. This method has no effect
	 * for items not added to manager or to items that has not been
	 * registered. This method calls @link itemAboutToBeUnregistered @endlink
	 * and @link itemUnregisterd @endlink.
	 */
	void unregisterItem(Item item)
	{
		if (!ItemsWithDetails.contains(item))
			return;
		if (!Items.contains(item))
			return;

		itemAboutToBeUnregisterd(item);
		ItemsWithDetails.removeAll(item);
		itemUnregistered(item);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads all items from configuration file.
	 *
	 * Loads all items from configuration file. Uses @link storageNodeItemName @endlink
	 * to get name of subnodes. Items are loaded by Item::loadFromStorage static method.
	 * Every loads will cause @link itemAboutToBeAdded @endlink and @link itemAdded
	 * @endlink methods to be called.
	 */
	virtual void load()
	{
		if (!isValidStorage())
			return;

		StorableObject::load();

		QDomElement itemsNode = storage()->point();
		if (itemsNode.isNull())
			return;

		QList<QDomElement> itemElements = storage()->storage()->getNodes(itemsNode, storageNodeItemName());
		foreach (QDomElement itemElement, itemElements)
		{
			StoragePoint *storagePoint = new StoragePoint(storage()->storage(), itemElement);

			QUuid uuid = storagePoint->point().attribute("uuid");
			if (!uuid.isNull())
			{
				Item item = byUuid(uuid); // this method loads
				item.data()->setStorage(storagePoint);
			}
		}
	}

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores all items to configuration file.
	 *
	 * Stores all items to configuration file. Items are stored by their
	 * store method. If an item is new (was not loaded from storage) it creates its
	 * own node by its createStoragePoint method.
	 */
	virtual void store()
	{
		ensureLoaded();

		foreach (Item item, Items)
			item.ensureStored();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns item by index.
	 * @param index index of item to return
	 * @return item with given index
	 *
	 * Returns item with given index. Indexes are arbitrary and can
	 * change when items are added or removed. This method returns the
	 * same item as items().at(index) would return.
	 *
	 * When index is out of range Item::null value is returned.
	 *
	 * This methods works on registered items.
	 */
	Item byIndex(unsigned int index)
	{
		ensureLoaded();

		if (index < 0 || index >= count())
			return Item::null;

		return ItemsWithDetails.at(index);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns item by uuid.
	 * @param uuid uuid of item to return
	 * @param create when true, new item can be created
	 * @return item with given uuid
	 *
	 * Returns item with given uuid. Uuid are arbitrary but can not
	 * change (even between application restarts).
	 *
	 * When no item with given uuid is found and create is true, new item is created
	 * and added to manager.
	 */
	Item byUuid(const QUuid &uuid, bool create = true)
	{
		foreach (Item item, Items)
			if (item.uuid() == uuid)
				return item;

		if (!create)
			return Item::null;

		Item item = Item::create();
		item.setUuid(uuid);
		item.data()->setState(StateNotLoaded);

		addItem(item);

		return item;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns index of given item.
	 * @param item item to find index
	 * @return index of given item
	 *
	 * Returns index of given item (or -1, if item is not found).
	 * Indexes are arbitrary and can change when items are added
	 * or removed. This method returns the same index as
	 * items().indexOf(item) would return.
	 *
	 * This methods works on registered items.
	 */
	unsigned int indexOf(Item item)
	{
		ensureLoaded();
		return ItemsWithDetails.indexOf(item);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns number of loaded items.
	 * @return number of loaded items
	 *
	 * Return number of loaded items.
	 *
	 * This methods works on registered items.
	 */
	unsigned int count()
	{
		ensureLoaded();
		return ItemsWithDetails.count();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns list of all items.
	 * @return list of all items
	 *
	 * Return list of all items.
	 */
	const QList<Item> allItems()
	{
		ensureLoaded();
		return Items;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns list of registered items.
	 * @return list of registered items
	 *
	 * Return list of registered items.
	 */
	const QList<Item> items()
	{
		ensureLoaded();
		return ItemsWithDetails;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Adds new item to list.
	 * @param item new item to be added
	 *
	 * Adds new item to list. If item is already added this method does nothing.
	 * This method calls @link itemAboutToBeAdded @endlink. If item has details
	 * it is also registered.
	 */
	void addItem(Item item)
	{
		ensureLoaded();

		if (Items.contains(item))
			return;

		itemAboutToBeAdded(item);

		Items.append(item);
		
		itemAdded(item);
	
		if (item.details())
			registerItem(item);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removed item from list.
	 * @param item item to be removed
	 *
	 * If item is not on list this method does nothing.
	 * Removes item from list. If item has details it is unregistered.
	 * This method calls @link itemAboutToBeRemoved @endlink.
	 *
	 * Item is removed from storage.
	 */
	void removeItem(Item item)
	{
		ensureLoaded();

		if (!Items.contains(item))
			return;

		itemAboutToBeRemoved(item);

		if (item.details())
			unregisterItem(item);
		Items.removeAll(item);

		item.remove();

		itemRemoved(item);
	}

};

/**
 * @}
 */

#endif // MANAGER_H
