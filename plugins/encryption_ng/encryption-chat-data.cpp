/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

EncryptionChatData::EncryptionChatData(const Chat &chat, QObject *parent) :
		QObject(parent), MyChat(chat), Encrypt(true)
{
	Encrypt = MyChat.property("encryption-ng:Encrypt", true).toBool();
	importEncrypt(); // this is only done once
}

EncryptionChatData::~EncryptionChatData()
{
}

void EncryptionChatData::importEncrypt()
{
	ContactSet contacts = MyChat.contacts();
	if (1 != contacts.size())
		return;

	Contact contact = *contacts.constBegin();
	QString encryptionEnabled = contact.ownerBuddy().customData("encryption_enabled");
	contact.ownerBuddy().removeCustomData("encryption_enabled");

	if (encryptionEnabled == "false")
		Encrypt = false;
}

void EncryptionChatData::setEncrypt(bool encrypt)
{
	if (!MyChat || Encrypt == encrypt)
		return;

	Encrypt = encrypt;

	if (!Encrypt)
		MyChat.addProperty("encryption-ng:Encrypt", false, CustomProperties::Storable);
	else
		MyChat.removeProperty("encryption-ng:Encrypt");
}

void EncryptionChatData::setLastEncryptionProviderName(const QString &lastEncryptionProviderName)
{
	MyChat.addProperty("encryption-ng:LastEncryptionProviderName", lastEncryptionProviderName, CustomProperties::Storable);
}

QString EncryptionChatData::lastEncryptionProviderName() const
{
	return MyChat.property("encryption-ng:LastEncryptionProviderName", QString()).toString();
}

void EncryptionChatData::setEncryptor(Encryptor *encryptor)
{
	ChatEncryptor = encryptor;
}

Encryptor * EncryptionChatData::encryptor() const
{
	return ChatEncryptor.data();
}

void EncryptionChatData::setDecryptor(Decryptor *decryptor)
{
	ChatDecryptor = decryptor;
}

Decryptor * EncryptionChatData::decryptor() const
{
	return ChatDecryptor.data();
}

#include "moc_encryption-chat-data.cpp"
