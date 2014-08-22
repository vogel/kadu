/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QtAlgorithms>

#include "core/core.h"
#include "storage/custom-properties.h"
#include "storage/storage-point-factory.h"

#include "storable-object.h"

StorableObject::StorableObject() :
		State(StateNew)
{
	Properties = new CustomProperties();
}

StorableObject::~StorableObject()
{
	delete Properties;
}

std::shared_ptr<StoragePoint> StorableObject::createStoragePoint()
{
	auto parent = storageParent();
	return Core::instance()->storagePointFactory()->createStoragePoint(storageNodeName(), parent ? parent->storage().get() : nullptr);
}

void StorableObject::setStorage(const std::shared_ptr<StoragePoint> &storage)
{
	State = StateNotLoaded;
	Storage = storage;
}

bool StorableObject::isValidStorage()
{
	return storage() && storage()->storage();
}

const std::shared_ptr<StoragePoint> & StorableObject::storage()
{
	if (!Storage)
		Storage = createStoragePoint();

	return Storage;
}

void StorableObject::store()
{
	ensureLoaded();
	Properties->storeTo(storage());
}

bool StorableObject::shouldStore()
{
	return true;
}

void StorableObject::load()
{
	State = StateLoaded;
	Properties->loadFrom(storage());
}

void StorableObject::ensureLoaded()
{
	if (StateNotLoaded == State)
		load();
}

void StorableObject::ensureStored()
{
	ensureLoaded();

	if (shouldStore())
		store();
	else
		removeFromStorage();
}

void StorableObject::removeFromStorage()
{
	if (!Storage)
		return;

	Storage->point().parentNode().removeChild(Storage->point());
	Storage.reset();
}

void StorableObject::storeValue(const QString &name, const QVariant value)
{
	Storage->storeValue(name, value);
}

void StorableObject::storeAttribute(const QString &name, const QVariant value)
{
	Storage->storeAttribute(name, value);
}

void StorableObject::removeValue(const QString& name)
{
	Storage->removeValue(name);
}

void StorableObject::removeAttribute(const QString& name)
{
	Storage->removeAttribute(name);
}

CustomProperties * StorableObject::customProperties() const
{
	return Properties;
}
