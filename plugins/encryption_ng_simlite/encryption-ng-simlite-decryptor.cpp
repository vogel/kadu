/*
 * Copyright 2007, 2008, 2009 Tomasz Kazmierczak
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QTextCodec>

#include "chat/chat.h"
#include "message/raw-message.h"
#include "plugins/encryption_ng/encryption-manager.h"
#include "plugins/encryption_ng/encryption-provider-manager.h"
#include "plugins/encryption_ng/keys/key.h"
#include "plugins/encryption_ng/keys/keys-manager.h"

#include "configuration/encryption-ng-simlite-configuration.h"
#include "encryption-ng-simlite-common.h"
#include "pkcs1_certificate.h"

#include "encryption-ng-simlite-decryptor.h"

EncryptioNgSimliteDecryptor::EncryptioNgSimliteDecryptor(const Account &account, EncryptionProvider *provider, QObject *parent) :
		Decryptor(provider, parent), MyAccount(account)
{
	connect(KeysManager::instance(), SIGNAL(keyAdded(Key)), this, SLOT(keyUpdated(Key)));
	connect(KeysManager::instance(), SIGNAL(keyUpdated(Key)), this, SLOT(keyUpdated(Key)));
	connect(KeysManager::instance(), SIGNAL(keyRemoved(Key)), this, SLOT(keyUpdated(Key)));

	updateKey();
}

EncryptioNgSimliteDecryptor::~EncryptioNgSimliteDecryptor()
{
	disconnect(KeysManager::instance(), 0, this, 0);
}

void EncryptioNgSimliteDecryptor::keyUpdated(const Key &key)
{
	if (key.keyContact() == MyAccount.accountContact() && key.keyType() == "simlite_private")
		updateKey();
}

void EncryptioNgSimliteDecryptor::updateKey()
{
	Valid = false;
	DecodingKey = QCA::PrivateKey();

	Key key = KeysManager::instance()->byContactAndType(MyAccount.accountContact(), "simlite_private", ActionReturnNull);
	if (key.isNull() || key.isEmpty())
		return;

	DecodingKey = getPrivateKey(key);
}

QCA::PrivateKey EncryptioNgSimliteDecryptor::getPrivateKey(const Key &key)
{
	QByteArray keyData = key.key().toByteArray().trimmed();
	if (!keyData.startsWith(BEGIN_RSA_PRIVATE_KEY) || !keyData.endsWith(END_RSA_PRIVATE_KEY))
	{
		Valid = false;
		return QCA::PrivateKey();
	}

	keyData = keyData.replace(BEGIN_RSA_PRIVATE_KEY, "");
	keyData = keyData.replace(END_RSA_PRIVATE_KEY, "");
	keyData = keyData.replace('\r', "");
	keyData = keyData.replace('\n', "");
	keyData = keyData.replace(' ', "");

	QCA::SecureArray certificate;

	QCA::Base64 decoder;
	decoder.setLineBreaksEnabled(true);
	certificate = decoder.decode(keyData);

	// just some fake security added
	keyData.fill(' ', keyData.size());
	keyData.clear();

	if (!decoder.ok())
	{
		Valid = false;
		return QCA::PrivateKey();
	}

	PKCS1Certificate::ConversionStatus status;
	PKCS1Certificate pkcs1;

	QCA::PrivateKey privateKey = pkcs1.privateKeyFromDER(certificate, status);
	if (PKCS1Certificate::OK != status)
	{
		Valid = false;
		return QCA::PrivateKey();
	}

	if (!privateKey.canDecrypt())
	{
		Valid = false;
		return QCA::PrivateKey();
	}

	Valid = true;
	return privateKey;
}

RawMessage EncryptioNgSimliteDecryptor::decrypt(const RawMessage &rawMessage, Chat chat, bool *ok)
{
	if (ok)
		*ok = false;

	if (!Valid)
		return rawMessage;

	//check if the message has at least the length of the shortest possible encrypted message
	if (rawMessage.rawPlainContent().length() < 192 && rawMessage.rawXmlContent().length() < 192)
		return rawMessage;

	auto data = rawMessage.rawPlainContent();

	//decode the message from the Base64 encoding
	QCA::Base64 decoder(QCA::Decode);
	QCA::SecureArray decodedMessage = decoder.stringToArray(data);

	if (!decoder.ok())
	{
		// hack for Kadu 0.12-1.0 compatibility
		data = rawMessage.rawXmlContent();
		decodedMessage = decoder.stringToArray(data);

		if (!decoder.ok())
			return rawMessage;
	}

	//extract the Blowfish key (first 128 characters)
	QCA::SecureArray encryptedBlowfishKey(decodedMessage.toByteArray().left(128));
	//and the encrypted message (the rest)
	QCA::SecureArray encryptedMessage(decodedMessage.toByteArray().mid(128));

	QCA::SymmetricKey blowfishKey;
	if (!DecodingKey.decrypt(encryptedBlowfishKey, &blowfishKey, QCA::EME_PKCS1_OAEP))
		return rawMessage;

	//recreate the initialization vector (should be the same as the one used for ciphering)
	QCA::InitializationVector iv(QByteArray(8, '\x00'));
	//now that we have the symmetric Blowfish key, we can decrypt the message;
	//create a 128 bit Blowfish cipher object using Cipher Block Chaining (CBC) mode,
	//with default padding and for decoding
	QCA::Cipher cipher(QString("blowfish"), QCA::Cipher::CBC, QCA::Cipher::DefaultPadding, QCA::Decode, blowfishKey, iv);

	//decipher the message (put the message into the decoding cipher object)
	QCA::SecureArray plainText = cipher.process(encryptedMessage);
	if (!cipher.ok())
		return rawMessage;

	//check whether the decrypted data length is at least the size of the header -
	//if not, then we have an invalid message
	if (plainText.size() < (int)sizeof(sim_message_header))
		return rawMessage;

	//extract the header from the decrypted data and check if the magic number is
	//correct
	sim_message_header head;
	memcpy(&head, plainText.constData(), sizeof(sim_message_header));
	if (head.magicFirstPart != SIM_MAGIC_V1_1 || head.magicSecondPart != SIM_MAGIC_V1_2)
		return rawMessage;

	if (ok)
		*ok = true;

	//the message has been decrypted! :D
	//put it into the input/output byte array
	QByteArray result;
	QTextCodec *cp1250Codec;
	bool useUtf8 = (head.flags & SIM_FLAG_UTF8_MESSAGE);
	if (!useUtf8)
	{
		cp1250Codec = QTextCodec::codecForName("CP1250");
		if (!cp1250Codec)
		{
			qWarning("Missing codec for \"CP1250\". Fix your system.");
			useUtf8 = true;
		}
	}

	if (useUtf8)
		result = plainText.constData() + sizeof(sim_message_header);
	else
		result = cp1250Codec->toUnicode(plainText.constData() + sizeof(sim_message_header)).toUtf8();

	if (chat)
	{
		bool supportUtf = (head.flags & SIM_FLAG_SUPPORT_UTF8);

		if (!supportUtf)
			chat.removeProperty("encryption-ng-simlite:SupportUtf");
		else
			chat.addProperty("encryption-ng-simlite:SupportUtf", true, CustomProperties::Storable);
	}

	// maybe this should not be done here
	if (EncryptionNgSimliteConfiguration::instance()->encryptAfterReceiveEncryptedMessage())
	{
		EncryptionProvider *encryptorProvider = EncryptionProviderManager::instance()->byName("simlite");
		EncryptionManager::instance()->setEncryptionProvider(chat, encryptorProvider);
	}

	return {result};
}

#include "moc_encryption-ng-simlite-decryptor.cpp"
