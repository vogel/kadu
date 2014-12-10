/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-set.h"

#include "plugins/encryption_ng/keys/keys-manager.h"

#include "encryption-ng-simlite-decryptor.h"
#include "encryption-ng-simlite-encryptor.h"
#include "encryption-ng-simlite-message-filter.h"

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

	connect(KeysManager::instance(), SIGNAL(keyAdded(Key)), this, SLOT(keyUpdated(Key)));
	connect(KeysManager::instance(), SIGNAL(keyUpdated(Key)), this, SLOT(keyUpdated(Key)));
	connect(KeysManager::instance(), SIGNAL(keyRemoved(Key)), this, SLOT(keyUpdated(Key)));
}

EncryptioNgSimliteProvider::~EncryptioNgSimliteProvider()
{
	triggerAllAccountsUnregistered();
}

void EncryptioNgSimliteProvider::setMessageFilter(EncryptionNgSimliteMessageFilter *messageFilter)
{
	if (MessageFilter)
		disconnect(MessageFilter.data(), 0, this, 0);

	MessageFilter = messageFilter;

	if (MessageFilter)
		connect(MessageFilter.data(), SIGNAL(keyReceived(Contact,QString,QByteArray)), this, SIGNAL(keyReceived(Contact,QString,QByteArray)));
}

void EncryptioNgSimliteProvider::accountRegistered(Account account)
{
	EncryptioNgSimliteDecryptor *accountDecryptor = new EncryptioNgSimliteDecryptor(account, this, this);
	Decryptors.insert(account, accountDecryptor);
}

void EncryptioNgSimliteProvider::accountUnregistered(Account account)
{
	if (Decryptors.contains(account))
	{
		EncryptioNgSimliteDecryptor *decryptor = Decryptors.take(account);
		delete decryptor;
	}
}

QString EncryptioNgSimliteProvider::name() const
{
	return "simlite";
}

QString EncryptioNgSimliteProvider::displayName() const
{
	return tr("Simlite");
}

bool EncryptioNgSimliteProvider::canDecrypt(const Chat &chat) const
{
	if (1 != chat.contacts().size())
		return false;

	auto decryptor = Decryptors.value(chat.chatAccount());
	return decryptor
		? decryptor->isValid()
		: false;
}

bool EncryptioNgSimliteProvider::canEncrypt(const Chat &chat) const
{
	if (1 != chat.contacts().size())
		return false;

	Key key = KeysManager::instance()->byContactAndType(*chat.contacts().constBegin(), "simlite", ActionReturnNull);
	return !key.isNull() && !key.isEmpty();
}

Decryptor * EncryptioNgSimliteProvider::acquireDecryptor(const Chat &chat)
{
	if (1 != chat.contacts().size())
		return 0;

	return Decryptors.value(chat.chatAccount());
}

Encryptor * EncryptioNgSimliteProvider::acquireEncryptor(const Chat &chat)
{
	if (1 != chat.contacts().size())
		return 0;

	EncryptioNgSimliteEncryptor *encryptor = new EncryptioNgSimliteEncryptor(*chat.contacts().constBegin(), this, this);
	if (!encryptor->isValid())
	{
		delete encryptor;
		return 0;
	}

	return encryptor;
}

void EncryptioNgSimliteProvider::releaseDecryptor(const Chat &chat, Decryptor *decryptor)
{
	Q_UNUSED(chat)
	Q_UNUSED(decryptor)
}

void EncryptioNgSimliteProvider::releaseEncryptor(const Chat &chat, Encryptor *encryptor)
{
	Q_UNUSED(chat)
	delete encryptor;
}

void EncryptioNgSimliteProvider::keyUpdated(Key key)
{
	Chat chat = ChatTypeContact::findChat(key.keyContact(), ActionReturnNull);
	if (!chat)
		return;

	emit canEncryptChanged(chat);
}

#include "moc_encryption-ng-simlite-provider.cpp"
