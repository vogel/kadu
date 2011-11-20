/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef BASE_STATUS_CONTAINER_H
#define BASE_STATUS_CONTAINER_H

#include "status/status-container.h"

#include "exports.h"

class StorableObject;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StorableStatusContainer
 * @author Rafał 'Vogel' Malinowski
 * @short Status container that can store and load status from storable object.
 * @see StorableObject
 * @see StatusSetter
 *
 * This class is status container that can store and load status from storable object.
 */
class KADUAPI StorableStatusContainer : public StatusContainer
{
	Q_OBJECT

	StorableObject *MyStorableObject;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new StorableStatusContainer instance.
	 * @param storableObject storable object to store status in
	 *
	 * This constructor creates new StorableStatusContainer instance associated with given storableObject.
	 * Each status loading and storing will be performed using this storable object.
	 */
	explicit StorableStatusContainer(StorableObject *storableObject);
	virtual ~StorableStatusContainer();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return status stored in storable object.
	 * @return stored status.
	 *
	 * Return status stored in storable object.
	 */
	virtual Status loadStatus();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Stores given status in storable object.
	 * @param status status to store
	 *
	 * Stores given status in storable object.
	 */
	virtual void storeStatus(Status status);

};

/**
 * @}
 */

#endif // BASE_STATUS_CONTAINER_H
