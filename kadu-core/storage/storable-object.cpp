/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QtAlgorithms>

#include "storage/custom-properties.h"

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

void StorableObject::setStorage(const QSharedPointer<StoragePoint> &storage)
{
	State = StateNotLoaded;
	Storage = storage;
}

bool StorableObject::isValidStorage()
{
	return storage() && storage()->storage();
}

const QSharedPointer<StoragePoint> & StorableObject::storage()
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
	Storage.clear();
}

void StorableObject::storeValue(const QString &name, const QVariant value)
{
	Storage->storage()->createTextNode(Storage->point(), name, value.toString());
}

void StorableObject::storeAttribute(const QString &name, const QVariant value)
{
	Storage->point().setAttribute(name, value.toString());
}

void StorableObject::removeValue(const QString& name)
{
	Storage->storage()->removeNode(Storage->point(), name);
}

void StorableObject::removeAttribute(const QString& name)
{
	Storage->point().removeAttribute(name);
}

CustomProperties * StorableObject::customProperties() const
{
	return Properties;
}
