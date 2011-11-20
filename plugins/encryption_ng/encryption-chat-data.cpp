/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "contacts/contact-set.h"
#include "decryptor.h"
#include "encryptor.h"

#include "encryption-chat-data.h"

EncryptionChatData::EncryptionChatData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent) :
		ModuleData(moduleName, parent, qobjectParent), ChatEncryptor(0), ChatDecryptor(0), Encrypt(EncryptStateDefault)
{
}

EncryptionChatData::~EncryptionChatData()
{
}

EncryptionChatData::EncryptState EncryptionChatData::importEncrypt()
{
	EncryptState result = EncryptStateDefault;
	StorableObject *chatStorage = storageParent();
	ChatShared *chat = dynamic_cast<ChatShared *>(chatStorage);

	if (!chat)
		return result;

	ContactSet contacts = chat->contacts();
	if (1 != contacts.size())
		return result;

	Contact contact = *contacts.constBegin();
	QString encryptionEnabled = contact.ownerBuddy().customData("encryption_enabled");
	contact.ownerBuddy().removeCustomData("encryption_enabled");

	if (encryptionEnabled == "false")
		result = EncryptStateDisabled;
	else if (encryptionEnabled == "true")
		result = EncryptStateEnabled;

	return result;
}

void EncryptionChatData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	Encrypt = hasValue("Encrypt")
			? (loadValue<bool>("Encrypt")
				? EncryptStateEnabled
				: EncryptStateDisabled)
			: importEncrypt();
}

void EncryptionChatData::store()
{
	if (!isValidStorage())
		return;

	Q_ASSERT(Encrypt != EncryptStateDefault);

	storeValue("Encrypt", (Encrypt == EncryptStateEnabled) ? true : false);
}

bool EncryptionChatData::shouldStore()
{
	ensureLoaded();

	return ModuleData::shouldStore() && (Encrypt != EncryptStateDefault);
}

QString EncryptionChatData::name() const
{
	return QLatin1String("encryption-ng");
}

void EncryptionChatData::encryptorDestroyed()
{
	ChatEncryptor = 0;
}

void EncryptionChatData::decryptorDestroyed()
{
	ChatDecryptor = 0;
}

void EncryptionChatData::setEncryptor(Encryptor *encryptor)
{
	if (ChatEncryptor)
		disconnect(ChatEncryptor, SIGNAL(destroyed()), this, SLOT(encryptorDestroyed()));

	ChatEncryptor = encryptor;

	if (ChatEncryptor)
		connect(ChatEncryptor, SIGNAL(destroyed()), this, SLOT(encryptorDestroyed()));
}

Encryptor * EncryptionChatData::encryptor()
{
	return ChatEncryptor;
}

void EncryptionChatData::setDecryptor(Decryptor *decryptor)
{
	if (ChatDecryptor)
		disconnect(ChatDecryptor, SIGNAL(destroyed()), this, SLOT(decryptorDestroyed()));

	ChatDecryptor = decryptor;

	if (ChatDecryptor)
		connect(ChatDecryptor, SIGNAL(destroyed()), this, SLOT(decryptorDestroyed()));
}

Decryptor * EncryptionChatData::decryptor()
{
	return ChatDecryptor;
}
