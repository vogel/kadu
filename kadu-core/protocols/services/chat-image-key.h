/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_IMAGE_KEY_H
#define CHAT_IMAGE_KEY_H

#include <QtCore/QPair>

#include "exports.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class ChatImageKey
 * @short Simple class for storing key (id) of chat image.
 * @todo Currently it has Gadu Gadu implementation, but it could be easily extended to support other protocols and methods too.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class stores information about chat image that is transferred between clients. For now it is only for Gadu Gadu, but this class could be
 * abstracted and extended to support Gadu Gadu and others protocols.
 *
 * Method toString() is used to create string representation of key. It must be unique per image as it is used in HTML representation of messages
 * to create placeholder for image that is not yet downloaded and to allow it to be replaced with real image after downloading.
 */
class KADUAPI ChatImageKey : public QPair<quint32, quint32>
{
public:
	/**
	 * @short Create new instance of ChatImageKey for given Gadu Gadu chat image.
	 * @author Rafał 'Vogel' Malinowski
	 * @param size size of Gadu Gadu image in bytes
	 * @param crc32 CRC32 checksum of Gadu Gadu image
	 */
	ChatImageKey(quint32 size, quint32 crc32);

	/**
	 * @short Return true if key is null.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if key is null
	 *
	 * Key is null when size and crc32 are both null.
	 */
	bool isNull() const;

	/**
	 * @short Return string representation of key.
	 * @author Rafał 'Vogel' Malinowski
	 * @return string representation of key
	 *
	 * Returned value must be unique per image as it is used in HTML representation of messages to create placeholder for image that is not yet
	 * downloaded and to allow it to be replaced with real image after downloading.
	 */
	QString toString() const;

	/**
	 * @short Return size of Gadu Gadu image in bytes.
	 * @author Rafał 'Vogel' Malinowski
	 * @return size of Gadu Gadu image in bytes
	 */
	quint32 size() const;

	/**
	 * @short Return CRC32 checksum of Gadu Gadu image.
	 * @author Rafał 'Vogel' Malinowski
	 * @return CRC32 checksum of Gadu Gadu image
	 */
	quint32 crc32() const;

};

/**
 * @}
 */

#endif // CHAT_IMAGE_KEY_H
