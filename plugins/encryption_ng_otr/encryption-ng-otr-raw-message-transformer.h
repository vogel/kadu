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

#ifndef ENCRYPTION_NG_OTR_RAW_MESSAGE_TRANSFORMER_H
#define ENCRYPTION_NG_OTR_RAW_MESSAGE_TRANSFORMER_H

#include <QtCore/QWeakPointer>

#include "protocols/services/raw-message-transformer.h"

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class EncryptionNgOtrAppOpsWrapper;
class EncryptionNgOtrNotifier;
class EncryptionNgOtrPrivateKeyService;
class EncryptionNgOtrUserState;

class EncryptionNgOtrRawMessageTransformer: public RawMessageTransformer
{
	EncryptionNgOtrUserState *UserState;

	QWeakPointer<EncryptionNgOtrAppOpsWrapper> OtrAppOpsWrapper;
	QWeakPointer<EncryptionNgOtrNotifier> OtrNotifier;
	QWeakPointer<EncryptionNgOtrPrivateKeyService> OtrPrivateKeyService;

	QByteArray transformReceived(const QByteArray &messageContent, const Message &message);
	QByteArray transformSent(const QByteArray &messageContent, const Message &message);

public:
	explicit EncryptionNgOtrRawMessageTransformer();
	virtual ~EncryptionNgOtrRawMessageTransformer();

	void setUserState(EncryptionNgOtrUserState *userState);

	void setEncryptionNgOtrAppOpsWrapper(EncryptionNgOtrAppOpsWrapper *encryptionNgOtrAppOpsWrapper);
	void setEncryptionNgOtrNotifier(EncryptionNgOtrNotifier *encryptionNgOtrNotifier);
	void setEncryptionNgOtrPrivateKeyService(EncryptionNgOtrPrivateKeyService *encryptionNgOtrPrivateKeyService);

	virtual QByteArray transform(const QByteArray &messageContent, const Message &message);

};

#endif // ENCRYPTION_NG_OTR_RAW_MESSAGE_TRANSFORMER_H
