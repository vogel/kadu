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

#include "decryptor-wrapper.h"
#include "encryption-provider.h"
#include "encryptor.h"

#include "encryption-provider-manager.h"
#include <chat/chat-manager.h>

EncryptionProviderManager * EncryptionProviderManager::Instance = 0;

void EncryptionProviderManager::createInstance()
{
	Instance = new EncryptionProviderManager();
}

void EncryptionProviderManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptionProviderManager::EncryptionProviderManager()
{
}

EncryptionProviderManager::~EncryptionProviderManager()
{
}

void EncryptionProviderManager::registerProvider(EncryptionProvider *provider)
{
	Providers.append(provider);

	connect(provider, SIGNAL(keyReceived(Contact,QByteArray)), this, SIGNAL(keyReceived(Contact,QByteArray)));

	foreach (const Chat &chat, ChatManager::instance()->items())
	{
		emit canDecryptChanged(chat);
		emit canEncryptChanged(chat);
	}
}

void EncryptionProviderManager::unregisterProvider(EncryptionProvider *provider)
{
	Providers.removeAll(provider);

	disconnect(provider, SIGNAL(keyReceived(Contact,QByteArray)), this, SIGNAL(keyReceived(Contact,QByteArray)));

	foreach (const Chat &chat, ChatManager::instance()->items())
	{
		emit canDecryptChanged(chat);
		emit canEncryptChanged(chat);
	}
}

bool EncryptionProviderManager::canDecrypt(const Chat &chat)
{
	foreach (EncryptionProvider *provider, Providers)
		if (provider->canDecrypt(chat))
			return true;

	return false;
}

bool EncryptionProviderManager::canEncrypt(const Chat &chat)
{
	foreach (EncryptionProvider *provider, Providers)
		if (provider->canEncrypt(chat))
			return true;

	return false;
}

Decryptor * EncryptionProviderManager::decryptor(const Chat &chat)
{
	foreach (EncryptionProvider *provider, Providers)
	{
		Decryptor *result = provider->decryptor(chat);
		if (result)
			return result;
	}

	return 0;
}

Decryptor * EncryptionProviderManager::decryptorWrapper(const Chat& chat)
{
	DecryptorWrapper *result = new DecryptorWrapper();

	foreach (EncryptionProvider *provider, Providers)
	{
		Decryptor *decryptor = provider->decryptor(chat);
		if (decryptor)
			result->addDecryptor(decryptor);
	}

	return result;
}

Encryptor * EncryptionProviderManager::encryptor(const Chat &chat)
{
	foreach (EncryptionProvider *provider, Providers)
	{
		Encryptor *result = provider->encryptor(chat);
		if (result)
			return result;
	}

	return 0;
}
