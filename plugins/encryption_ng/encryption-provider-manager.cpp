/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy-manager.h"
#include "chat/chat-manager.h"
#include "gui/windows/message-dialog.h"

#include "keys/keys-manager.h"

#include "decryptor-wrapper.h"
#include "encryption-chat-data.h"
#include "encryption-manager.h"
#include "encryption-provider.h"
#include "encryptor.h"

#include "encryption-provider-manager.h"

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

	connect(provider, SIGNAL(keyReceived(Contact,QString,QByteArray)), this, SLOT(keyReceived(Contact,QString,QByteArray)));
	connect(provider, SIGNAL(canDecryptChanged(Chat)), this, SIGNAL(canDecryptChanged(Chat)));
	connect(provider, SIGNAL(canEncryptChanged(Chat)), this, SIGNAL(canEncryptChanged(Chat)));

	foreach (const Chat &chat, ChatManager::instance()->items())
	{
		emit canDecryptChanged(chat);
		emit canEncryptChanged(chat);
	}

	emit providerRegistered(provider);
}

void EncryptionProviderManager::unregisterProvider(EncryptionProvider *provider)
{
	if (Providers.removeAll(provider) <= 0)
		return;

	disconnect(provider, 0, this, 0);

	foreach (const Chat &chat, ChatManager::instance()->items())
	{
		emit canDecryptChanged(chat);
		emit canEncryptChanged(chat);
	}

	emit providerUnregistered(provider);
}

EncryptionProvider * EncryptionProviderManager::byName(const QString &name) const
{
	foreach (EncryptionProvider *provider, Providers)
		if (provider->name() == name)
			return provider;

	return 0;
}

bool EncryptionProviderManager::canDecrypt(const Chat &chat) const
{
	foreach (EncryptionProvider *provider, Providers)
		if (provider->canDecrypt(chat))
			return true;

	return false;
}

bool EncryptionProviderManager::canEncrypt(const Chat &chat) const
{
	return (0 != defaultEncryptorProvider(chat));
}

EncryptionProvider * EncryptionProviderManager::defaultEncryptorProvider(const Chat &chat) const
{
	if (!chat)
		return 0;

	EncryptionChatData *encryptionChatData = EncryptionManager::instance()->chatEncryption(chat);
	QString lastEncryptionProviderName = encryptionChatData->lastEncryptionProviderName();
	if (!lastEncryptionProviderName.isEmpty())
		return byName(lastEncryptionProviderName);

	foreach (EncryptionProvider *provider, Providers)
		if (provider->canEncrypt(chat))
			return provider;

	return 0;
}

QString EncryptionProviderManager::name() const
{
	return QString();
}

QString EncryptionProviderManager::displayName() const
{
	return QString();
}

Decryptor * EncryptionProviderManager::acquireDecryptor(const Chat &chat)
{
	return new DecryptorWrapper(chat, this, this);
}

Encryptor * EncryptionProviderManager::acquireEncryptor(const Chat &chat)
{
	EncryptionProvider *provider = defaultEncryptorProvider(chat);
	if (!provider)
		return 0;

	return provider->acquireEncryptor(chat);
}

void EncryptionProviderManager::releaseDecryptor(const Chat &chat, Decryptor *decryptor)
{
	Q_UNUSED(chat)

	DecryptorWrapper *decryptorWrapper = qobject_cast<DecryptorWrapper *>(decryptor);
	if (!decryptorWrapper)
		return;

	QList<Decryptor *> decryptors = decryptorWrapper->decryptors();
	foreach (Decryptor *decryptor, decryptors)
		decryptor->provider()->releaseDecryptor(chat, decryptor);

	delete decryptorWrapper;
}

void EncryptionProviderManager::releaseEncryptor(const Chat &chat, Encryptor *encryptor)
{
	Q_UNUSED(chat)
	Q_UNUSED(encryptor)

	// should not get called, we just provide encryptors from other class
}

// I know it is not best place for invoking gui, please change it in future
void EncryptionProviderManager::keyReceived(const Contact &contact, const QString &keyType, const QByteArray &keyData)
{
	if (contact.isAnonymous())
		return; // just ignore anonymous contacts

	Key key = KeysManager::instance()->byContactAndType(contact, keyType, ActionReturnNull);
	// we already have this key
	if (key && key.key() == keyData)
		return;

	QString question = tr("Buddy %1 is sending you his public key.\nDo you want to save it?").arg(contact.display(true));
	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-question"), tr("Encryption"), question);
	dialog->addButton(QMessageBox::Yes, tr("Save"));
	dialog->addButton(QMessageBox::No, tr("Ignore"));

	if (dialog->ask())
	{
		key = KeysManager::instance()->byContactAndType(contact, keyType, ActionCreateAndAdd);
		key.setKey(keyData);
	}
}

#include "moc_encryption-provider-manager.cpp"
