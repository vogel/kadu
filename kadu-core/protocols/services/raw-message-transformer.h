/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MESSAGE_TRANSFORMER_H
#define MESSAGE_TRANSFORMER_H

#include "core/configurable-transformer.h"
#include "exports.h"

class QByteArray;

class Chat;
class Message;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class RawMessageTransformer
 * @short Interface to transform raw content of messages.
 * @author Rafał 'Vogel' Malinowski
 *
 * This interface allows arbitraty tranformations of raw content of messages - the one that is directly sent or received.
 */
class KADUAPI RawMessageTransformer : public ConfigurableTransformer<QByteArray, Message>
{

public:
	/**
	 * @short Transform raw content of message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param rawMessage raw content of message
	 * @param memssage message that corresponds to rawMessage
	 * @return transformed raw message content
	 */
	virtual QByteArray transform(const QByteArray &rawMessage, const Message &message) = 0;

};

/**
 * @}
 */

#endif // MESSAGE_TRANSFORMER_H
