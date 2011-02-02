/*
 * Copyright 2007, 2008, 2009 Tomasz Kazmierczak
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

#include <stdint.h>

#include "misc/coding-conversion.h"
#include "modules/encryption_ng/keys/key.h"
#include "modules/encryption_ng/keys/keys-manager.h"

#include "pkcs1_certificate.h"

#include "encryption-ng-simlite-decryptor.h"

#define BEGIN_RSA_PRIVATE_KEY "-----BEGIN RSA PRIVATE KEY-----"
#define END_RSA_PRIVATE_KEY "-----END RSA PRIVATE KEY-----"

#define BEGIN_RSA_PRIVATE_KEY_LENGTH strlen(BEGIN_RSA_PRIVATE_KEY)
#define END_RSA_PRIVATE_KEY_LENGTH strlen(END_RSA_PRIVATE_KEY)

#define SIM_MAGIC_V1_1 0x91
#define SIM_MAGIC_V1_2 0x23
typedef struct {
	unsigned char init[8];
	uint8_t magicFirstPart;
	uint8_t magicSecondPart;
	uint8_t flags;
} sim_message_header;

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
	disconnect(KeysManager::instance(), SIGNAL(keyAdded(Key)), this, SLOT(keyUpdated(Key)));
	disconnect(KeysManager::instance(), SIGNAL(keyUpdated(Key)), this, SLOT(keyUpdated(Key)));
	disconnect(KeysManager::instance(), SIGNAL(keyRemoved(Key)), this, SLOT(keyUpdated(Key)));
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

	keyData = keyData.mid(BEGIN_RSA_PRIVATE_KEY_LENGTH, keyData.length() - BEGIN_RSA_PRIVATE_KEY_LENGTH - END_RSA_PRIVATE_KEY_LENGTH).replace('\r', "").trimmed();

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

QByteArray EncryptioNgSimliteDecryptor::decrypt(const QByteArray &data, bool *ok)
{
	if (ok)
		*ok = false;

	if (!Valid)
		return data;

	//check if the message has at least the length of the shortest possible encrypted message
	if (data.length() < 192)
		return data;

	//decode the message from the Base64 encoding
	QCA::Base64 decoder(QCA::Decode);
	QCA::SecureArray decodedMessage = decoder.stringToArray(data);

	if (!decoder.ok())
		return data;

	//extract the Blowfish key (first 128 characters)
	QCA::SecureArray encryptedBlowfishKey(decodedMessage.toByteArray().left(128));
	//and the encrypted message (the rest)
	QCA::SecureArray encryptedMessage(decodedMessage.toByteArray().mid(128));

	QCA::SymmetricKey blowfishKey;
	if (!DecodingKey.decrypt(encryptedBlowfishKey, &blowfishKey, QCA::EME_PKCS1_OAEP))
		return data;

	//recreate the initialization vector (should be the same as the one used for ciphering)
	char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	QCA::InitializationVector iv(QByteArray(ivec, 8));
	//now that we have the symmetric Blowfish key, we can decrypt the message;
	//create a 128 bit Blowfish cipher object using Cipher Block Chaining (CBC) mode,
	//with default padding and for decoding
	QCA::Cipher cipher(QString("blowfish"), QCA::Cipher::CBC, QCA::Cipher::DefaultPadding, QCA::Decode, blowfishKey, iv);

	//decipher the message (put the message into the decoding cipher object)
	QCA::SecureArray plainText = cipher.process(encryptedMessage);
	if (!cipher.ok())
		return data;

	//check whether the decrypted data length is at least the size of the header -
	//if not, then we have an invalid message
	if (plainText.size() < (int)sizeof(sim_message_header))
		return data;

	//extract the header from the decrypted data and check if the magic number is
	//correct
	sim_message_header head;
	memcpy(&head, plainText.data(), sizeof(sim_message_header));
	if (head.magicFirstPart != SIM_MAGIC_V1_1 || head.magicSecondPart != SIM_MAGIC_V1_2)
		return data;

	if (ok)
		*ok = true;

	//the message has been decrypted! :D
	//put it into the input/output byte array
	return cp2unicode(&plainText.data()[sizeof(sim_message_header)]).toUtf8();
}
