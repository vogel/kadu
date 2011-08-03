/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_FORMATTER_H
#define GADU_FORMATTER_H

#include "../gadu-exports.h"
#include "../gadu-protocol.h"

class Account;
class FormattedMessage;

namespace GaduFormatter
{
	GADUAPI unsigned int computeFormatsSize(const FormattedMessage &message);
	GADUAPI unsigned char * createFormats(Account account, const FormattedMessage &message, unsigned int &size);

	GADUAPI QString createImageId(GaduProtocol::UinType sender, unsigned int size, unsigned int crc32);

	GADUAPI FormattedMessage createMessage(Account account, Contact contact, const QString &content, unsigned char *formats,
			unsigned int size, bool receiveImages);

}

#endif // GADU_FORMATTER_H
