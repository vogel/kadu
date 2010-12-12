/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QtGlobal>

#include "encryption-ng-ceasar-decryptor.h"
#include "encryption-ng-ceasar-encryptor.h"

#include "encryption-ng-ceasar-provider.h"

EncryptionNgCeasarProvider * EncryptionNgCeasarProvider::Instance = 0;

void EncryptionNgCeasarProvider::createInstance()
{
	Instance = new EncryptionNgCeasarProvider();
}

void EncryptionNgCeasarProvider::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptionNgCeasarProvider::EncryptionNgCeasarProvider()
{
	CeasarDecryptor = new EncryptionNgCeasarDecryptor();
	CeasarEncryptor = new EncryptionNgCeasarEncryptor();
}

EncryptionNgCeasarProvider::~EncryptionNgCeasarProvider()
{
	delete CeasarDecryptor;
	CeasarDecryptor = 0;
	delete CeasarEncryptor;
	CeasarEncryptor = 0;
}

bool EncryptionNgCeasarProvider::canDecrypt(const Chat &chat)
{
	Q_UNUSED(chat)

	return true;
}

bool EncryptionNgCeasarProvider::canEncrypt(const Chat &chat)
{
	Q_UNUSED(chat)

	return true;
}

Decryptor * EncryptionNgCeasarProvider::decryptor(const Chat &chat)
{
	Q_UNUSED(chat)

	return CeasarDecryptor;
}

Encryptor * EncryptionNgCeasarProvider::encryptor(const Chat &chat)
{
	Q_UNUSED(chat)

	return CeasarEncryptor;
}
