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

#ifndef OTR_RAW_MESSAGE_TRANSFORMER_H
#define OTR_RAW_MESSAGE_TRANSFORMER_H

#include <QtCore/QWeakPointer>

#include "protocols/services/raw-message-transformer.h"

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class OtrAppOpsWrapper;
class OtrOpData;
class OtrPrivateKeyService;
class OtrUserState;

class OtrRawMessageTransformer: public RawMessageTransformer
{
	OtrUserState *UserState;
	bool EnableFragments;

	QWeakPointer<OtrAppOpsWrapper> AppOpsWrapper;
	QWeakPointer<OtrPrivateKeyService> PrivateKeyService;

	QByteArray transformReceived(const QByteArray &messageContent, const Message &message);
	QByteArray transformSent(const QByteArray &messageContent, const Message &message);

	OtrOpData otrOpDataFromMessage(const Message &message) const;

public:
	explicit OtrRawMessageTransformer();
	virtual ~OtrRawMessageTransformer();

	void setUserState(OtrUserState *userState);
	void setEnableFragments(bool enableFragments);

	void setOtrAppOpsWrapper(OtrAppOpsWrapper *encryptionNgOtrAppOpsWrapper);
	void setOtrPrivateKeyService(OtrPrivateKeyService *encryptionNgOtrPrivateKeyService);

	virtual QByteArray transform(const QByteArray &messageContent, const Message &message);

};

#endif // OTR_RAW_MESSAGE_TRANSFORMER_H
