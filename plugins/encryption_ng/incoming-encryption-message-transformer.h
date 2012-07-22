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

#ifndef INCOMING_ENCRYPTION_MESSAGE_TRANSFORMER_H
#define INCOMING_ENCRYPTION_MESSAGE_TRANSFORMER_H

#include "message/message-transformer.h"

class EncryptionManager;

class IncomingEncryptionMessageTransformer : public MessageTransformer
{
	Q_OBJECT

	EncryptionManager *CurrentEncryptionManager;

public:
	explicit IncomingEncryptionMessageTransformer(EncryptionManager *encryptionManager);
	virtual ~IncomingEncryptionMessageTransformer();

	virtual QString transformMessage(const Chat &chat, const QString &message);

};

#endif // INCOMING_ENCRYPTION_MESSAGE_TRANSFORMER_H
