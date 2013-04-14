/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "encryption-ng-otr-op-data.h"

EncryptionNgOtrOpData::EncryptionNgOtrOpData() :
		AppOpsWrapper(0), PrivateKeyService(0)
{
}

EncryptionNgOtrOpData::~EncryptionNgOtrOpData()
{
}

void EncryptionNgOtrOpData::setAppOpsWrapper(EncryptionNgOtrAppOpsWrapper *appOpsWrapper)
{
	AppOpsWrapper = appOpsWrapper;
}

EncryptionNgOtrAppOpsWrapper * EncryptionNgOtrOpData::appOpsWrapper() const
{
	return AppOpsWrapper;
}

void EncryptionNgOtrOpData::setPrivateKeyService(EncryptionNgOtrPrivateKeyService *privateKeyService)
{
	PrivateKeyService = privateKeyService;
}

EncryptionNgOtrPrivateKeyService * EncryptionNgOtrOpData::privateKeyService() const
{
	return PrivateKeyService;
}

void EncryptionNgOtrOpData::setMessage(const Message &message)
{
	MyMessage = message;
}

Message EncryptionNgOtrOpData::message() const
{
	return MyMessage;
}
