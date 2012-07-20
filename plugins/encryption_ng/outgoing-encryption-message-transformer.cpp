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

#include "encryption-chat-data.h"
#include "encryption-manager.h"
#include "encryptor.h"

#include "outgoing-encryption-message-transformer.h"

OutgoingEncryptionMessageTransformer::OutgoingEncryptionMessageTransformer(EncryptionManager *encryptionManager) :
		MessageTransformer(encryptionManager), CurrentEncryptionManager(encryptionManager)
{
}

OutgoingEncryptionMessageTransformer::~OutgoingEncryptionMessageTransformer()
{
}

QString OutgoingEncryptionMessageTransformer::transformMessage(const Chat &chat, const QString &message)
{
	if (!chat)
		return message;

	EncryptionChatData *encryptionChatData = CurrentEncryptionManager->chatEncryption(chat);
	if (encryptionChatData && encryptionChatData->encryptor())
		return QString::fromUtf8(encryptionChatData->encryptor()->encrypt(message.toUtf8()));

	return message;
}
