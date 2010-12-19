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

#include "encryption-ng-simlite-provider.h"

EncryptioNgSimliteProvider * EncryptioNgSimliteProvider::Instance = 0;

void EncryptioNgSimliteProvider::createInstance()
{
	Instance = new EncryptioNgSimliteProvider();
}

void EncryptioNgSimliteProvider::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptioNgSimliteProvider::EncryptioNgSimliteProvider()
{
	triggerAllAccountsRegistered();
}

EncryptioNgSimliteProvider::~EncryptioNgSimliteProvider()
{
	triggerAllAccountsUnregistered();
}

void EncryptioNgSimliteProvider::accountRegistered(Account account)
{
	Q_UNUSED(account)
}

void EncryptioNgSimliteProvider::accountUnregistered(Account account)
{
	Q_UNUSED(account)
}

bool EncryptioNgSimliteProvider::canDecrypt(const Chat &chat)
{
	Q_UNUSED(chat)
	return false;
}

bool EncryptioNgSimliteProvider::canEncrypt(const Chat &chat)
{
	Q_UNUSED(chat)
	return false;
}

Decryptor * EncryptioNgSimliteProvider::decryptor(const Chat &chat)
{
	Q_UNUSED(chat)
	return 0;
}

Encryptor * EncryptioNgSimliteProvider::encryptor(const Chat &chat)
{
	Q_UNUSED(chat)
	return 0;
}
