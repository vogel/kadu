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

#include "modules/encryption_ng/keys/key.h"
#include "modules/encryption_ng/keys/keys-manager.h"

#include "pkcs1_certificate.h"

#include "encryption-ng-simlite-encryptor.h"

#define BEGIN_RSA_PUBLIC_KEY "-----BEGIN RSA PUBLIC KEY-----"
#define END_RSA_PUBLIC_KEY "-----END RSA PUBLIC KEY-----"

#define BEGIN_RSA_PUBLIC_KEY_LENGTH strlen(BEGIN_RSA_PUBLIC_KEY)
#define END_RSA_PUBLIC_KEY_LENGTH strlen(END_RSA_PUBLIC_KEY)

#define SIM_MAGIC_V1_1 0x91
#define SIM_MAGIC_V1_2 0x23
typedef struct {
	unsigned char init[8];
	uint8_t magicFirstPart;
	uint8_t magicSecondPart;
	uint8_t flags;
} sim_message_header;

EncryptioNgSimliteEncryptor::EncryptioNgSimliteEncryptor(const Contact &contact, EncryptionProvider *provider, QObject *parent) :
		Encryptor(provider, parent), MyContact(contact)
{
	connect(KeysManager::instance(), SIGNAL(keyAdded(Key)), this, SLOT(keyUpdated(Key)));
	connect(KeysManager::instance(), SIGNAL(keyUpdated(Key)), this, SLOT(keyUpdated(Key)));
	connect(KeysManager::instance(), SIGNAL(keyRemoved(Key)), this, SLOT(keyUpdated(Key)));

	updateKey();
}

EncryptioNgSimliteEncryptor::~EncryptioNgSimliteEncryptor()
{
	disconnect(KeysManager::instance(), SIGNAL(keyAdded(Key)), this, SLOT(keyUpdated(Key)));
	disconnect(KeysManager::instance(), SIGNAL(keyUpdated(Key)), this, SLOT(keyUpdated(Key)));
	disconnect(KeysManager::instance(), SIGNAL(keyRemoved(Key)), this, SLOT(keyUpdated(Key)));

}

void EncryptioNgSimliteEncryptor::keyUpdated(const Key &key)
{
	if (key.keyContact() == MyContact && key.keyType() == "simlite")
		updateKey();
}

void EncryptioNgSimliteEncryptor::updateKey()
{
	Valid = false;
	EncodingKey = QCA::PublicKey();

	Key key = KeysManager::instance()->byContactAndType(MyContact, "simlite", ActionReturnNull);
	if (key.isNull() || key.isEmpty())
		return;

	EncodingKey = getPublicKey(key);
}

QCA::PublicKey EncryptioNgSimliteEncryptor::getPublicKey(const Key &key)
{
	QByteArray keyData = key.key().toByteArray().trimmed();
	if (!keyData.startsWith(BEGIN_RSA_PUBLIC_KEY) || !keyData.endsWith(END_RSA_PUBLIC_KEY))
	{
		Valid = false;
		return QCA::PublicKey();
	}

	keyData = keyData.mid(BEGIN_RSA_PUBLIC_KEY_LENGTH, keyData.length() - BEGIN_RSA_PUBLIC_KEY_LENGTH - END_RSA_PUBLIC_KEY_LENGTH);

	QCA::SecureArray certificate;

	QCA::Base64 decoder(QCA::Decode);
	decoder.setLineBreaksEnabled(true);
	certificate = decoder.decode(keyData);
	certificate += decoder.final();

	// some fake security added
	keyData.fill(' ', keyData.size());
	keyData.clear();

	if (!decoder.ok())
	{
		Valid = false;
		return QCA::PublicKey();
	}

	PKCS1Certificate::ConversionStatus status;
	PKCS1Certificate pkcs1;

	QCA::PublicKey publicKey = pkcs1.publicKeyFromDER(certificate, status);
	if (PKCS1Certificate::OK != status)
	{
		Valid = false;
		return QCA::PublicKey();
	}

	if (!publicKey.canEncrypt())
	{
		Valid = false;
		return QCA::PublicKey();
	}

	Valid = true;
	return publicKey;
}

QByteArray EncryptioNgSimliteEncryptor::encrypt(const QByteArray &data)
{
	if (!Valid)
		return data;

	//generate a symmetric key for Blowfish (16 bytes in length)
	QCA::SymmetricKey blowfishKey(16);

	//encrypt the symmetric key using the RSA public key
	QCA::SecureArray encryptedBlowfishKey = EncodingKey.encrypt(blowfishKey, QCA::EME_PKCS1_OAEP);
	if (encryptedBlowfishKey.isEmpty())
		return data;

	//create an initialisation vector (8 zeros)
	char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	QCA::InitializationVector iv(QByteArray(ivec, 8));
	//encrypt the message using the Blowfish key:
	//create a 128 bit Blowfish cipher object using Cipher Block Chaining (CBC) mode,
	//with default padding and for encoding
	QCA::Cipher cipher(QString("blowfish"), QCA::Cipher::CBC, QCA::Cipher::DefaultPadding, QCA::Encode, blowfishKey, iv);

	//prepare the header
	sim_message_header head;
	memset(&head, 0, sizeof(head));
	//put the magic number into it
	head.magicFirstPart = SIM_MAGIC_V1_1;
	head.magicSecondPart = SIM_MAGIC_V1_2;
	//fill the iv in the header with some random bytes (using IV is a simple way)
	QCA::InitializationVector headIV(8);
	memcpy(head.init, headIV.data(), 8);

	//the actual encryption
	QByteArray encryptedData = QByteArray((const char *)&head, sizeof(sim_message_header)) + data;
	QCA::SecureArray encrypted = cipher.update(encryptedData);

	if (!cipher.ok())
		return data;

	//output the final block
	encrypted.append(cipher.final());
	if (!cipher.ok())
		return data;

	//build the encrypted message
	encrypted = encryptedBlowfishKey + encrypted;

	//encode it using Base64 algorithm
	QCA::Base64 encoder;
	encrypted = encoder.encode(encrypted);
//NOTE: this seems to break the message (and without it everything works fine)
//	encrypted += encoder.final();
	if (!encoder.ok())
		return data;

	//finally, put the encrypted message into the output QByteArray
	return encrypted.toByteArray();
}
