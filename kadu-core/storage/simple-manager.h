/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMPLE_MANAGER_H
#define SIMPLE_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/configuration-manager.h"
#include "storage/storable-object.h"

#include "exports.h"

template<class Item>
class KADUAPI SimpleManager : public StorableObject
{
	QList<Item> Items;

protected:
	SimpleManager()
	{
		setState(StateNotLoaded);
		ConfigurationManager::instance()->registerStorableObject(this);
	}

	virtual ~SimpleManager()
	{
		ConfigurationManager::instance()->unregisterStorableObject(this);
	}

	virtual StorableObject * storageParent()
	{
		return 0;
	}

	virtual QString storageNodeItemName() = 0;

	virtual void itemAboutToBeAdded(Item item) {}
	virtual void itemAdded(Item item) {}
	virtual void itemAboutToBeRemoved(Item item) {}
	virtual void itemRemoved(Item item) {}

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
			Item item = Item::loadFromStorage(storagePoint);

			addItem(item);
		}
	}

public:
	virtual void store()
	{
		ensureLoaded();

		foreach (Item item, Items)
			item.store();
	}

	Item byIndex(unsigned int index)
	{
		ensureLoaded();

		if (index < 0 || index >= count())
			return Item::null;

		return Items.at(index);
	}

	Item byUuid(const QUuid &uuid)
	{
		ensureLoaded();

		foreach (Item item, Items)
			if (item.uuid() == uuid)
				return item;

		return Item::null;
	}

	unsigned int indexOf(Item item)
	{
		ensureLoaded();
		return Items.indexOf(item);
	}

	unsigned int count()
	{
		ensureLoaded();
		return Items.count();
	}

	const QList<Item> items()
	{
		ensureLoaded();
		return Items;
	}

	void addItem(Item item)
	{
		ensureLoaded();

		if (Items.contains(item))
			return;

		itemAboutToBeAdded(item);
		Items.append(item);
		itemAdded(item);
	}

	void removeItem(Item item)
	{
		ensureLoaded();

		if (!Items.contains(item))
			return;

		itemAboutToBeRemoved(item);
		Items.removeAll(item);
		itemRemoved(item);

		item.remove();
	}

};

#endif // SIMPLE_MANAGER_H
