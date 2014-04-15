/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include "message/raw-message.h"

DecryptorWrapper::DecryptorWrapper(const Chat &chat, EncryptionProviderManager *providerManager, QObject *parent) :
		Decryptor(providerManager, parent), MyChat(chat)
{
	connect(providerManager, SIGNAL(providerRegistered(EncryptionProvider*)),
			this, SLOT(providerRegistered(EncryptionProvider*)));

	foreach (EncryptionProvider *provider, providerManager->providers())
		providerRegistered(provider);
}

void DecryptorWrapper::providerRegistered(EncryptionProvider *provider)
{
	Decryptor *decryptor = provider->acquireDecryptor(MyChat);
	if (decryptor)
		addDecryptor(decryptor);
}

void DecryptorWrapper::addDecryptor(Decryptor *decryptor)
{
	Decryptors.append(decryptor);
	connect(decryptor, SIGNAL(destroyed(QObject*)), this, SLOT(decryptorDestroyed(QObject*)));
}

void DecryptorWrapper::removeDecryptor(Decryptor *decryptor)
{
	Decryptors.removeAll(decryptor);
	disconnect(decryptor, 0, this, 0);
}

void DecryptorWrapper::decryptorDestroyed(QObject *decryptor)
{
	Decryptors.removeAll(static_cast<Decryptor *>(decryptor));
}

RawMessage DecryptorWrapper::decrypt(const RawMessage &data, Chat chat, bool *ok)
{
	RawMessage decrypted = data;

	if (ok)
		*ok = false;

	foreach (Decryptor *decryptor, Decryptors)
	{
		bool thisOk;
		decrypted = decryptor->decrypt(decrypted, chat, &thisOk);
		if (ok)
			*ok = *ok || thisOk;
	}

	return decrypted;
}

#include "moc_decryptor-wrapper.cpp"
