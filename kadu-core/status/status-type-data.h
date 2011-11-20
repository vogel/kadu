/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef STATUS_TYPE_DATA_H
#define STATUS_TYPE_DATA_H

#include <QtCore/QString>
#include <QtCore/QVariant>

#include "status/status-type-group.h"
#include "status/status-type.h"

#include "exports.h"

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StatusTypeData
 * @author Rafał 'Vogel' Malinowski
 * @short Extendend information about StatusType enum values.
 * @see StatusType
 * @see StatusTypeManager
 *
 * This class contains extended information about StatusType enums. If C++ allowed for enums with fields, this class
 * would be merged with StatusType.
 *
 * StatusTypeData instances can be only aquired by StatusTypeManager methods. Use instances of this class to get
 * additional info like status type name, display name, icon or type group.
 */
class KADUAPI StatusTypeData
{
	friend class StatusTypeManager;

	StatusType Type;
	QString Name;
	QString DisplayName;
	QString IconName;

	StatusTypeGroup TypeGroup;

	StatusTypeData(StatusType type, const QString &name, const QString &displayName, const QString &iconName, StatusTypeGroup typeGroup);

public:
	StatusTypeData();
	StatusTypeData(const StatusTypeData &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns StatusType enum value associated with this object.
	 * @return StatusType enum value associated with this object
	 *
	 * Returns StatusType enum value associated with this object.
	 */
	StatusType type() const { return Type; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns name of status type associated with this object.
	 * @return name of status type associated with this object
	 *
	 * Returns name of status type associated with this object. This name can be used to store
	 * status type in configuration files. StatusTypeData can be restored from name using
	 * StatusTypeManager::byName method.
	 */
	const QString & name() const { return Name; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns display name of status type associated with this object.
	 * @return display name of status type associated with this object
	 *
	 * Returns display name of status type associated with this object.
	 */
	const QString & displayName() const { return DisplayName; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns icon name associated with this status type.
	 * @return icon name associated with this status type
	 *
	 * Returns icon name associated with this status type.
	 */
	const QString & iconName() const { return IconName; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns StatusTypeGroup enum value associated with this status type.
	 * @return StatusTypeGroup enum value associated with this status type
	 *
	 * Returns StatusTypeGroup enum value associated with this status type.
	 */
	StatusTypeGroup typeGroup() const { return TypeGroup; }

};

/**
 * @addtogroup Status
 * @}
 */

#endif // STATUS_TYPE_DATA_H
