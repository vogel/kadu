/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef UUID_STORABLE_OBJECT_H
#define UUID_STORABLE_OBJECT_H

#include <QtCore/QUuid>

#include "storable-object.h"

#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */
/**
 * @class UuidStorableObject
 * @author Rafal 'Vogel' Malinowski
 * @short Object that can load itself from XML file and store data there. Object is identified by UUID.
 *
 * Many objects of this class can be stored under one master XML node in storage.
 * Each object is identified by unique identifier (called UUID) that never changes
 * after object is created.
 */
class KADUAPI UuidStorableObject : public StorableObject
{
	QUuid Uuid;

protected:
	virtual QSharedPointer<StoragePoint> createStoragePoint();

public:
	UuidStorableObject();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns unique identifier of this object.
	 * @return unique identifier of this object
	 *
	 * This method returns object of type QUuid that represens unique ID for this object.
	 * Each derivered class should ovveride that method to return really unique ID.
	 */
	const QUuid & uuid() const { return Uuid; }

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Updates value of unique identifier of this object.
	 * @return unique identifier of this object
	 *
	 * This method sets new unique ID for this object. Unique ID is
	 * represented by object of type QUuid.
	 */
	void setUuid(const QUuid &uuid) { Uuid = uuid; }

	virtual bool shouldStore();

};

/**
 * @}
 */

#endif // UUID_STORABLE_OBJECT_H
