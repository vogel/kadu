/*
  File name: kadu_encryption_simlite.cpp
  Copyright: (C) 2007, 2008 - 2009 Tomasz Kazmierczak

  Creation date: 2007-10-25
  Last modification date: 2009-05-11

  This file is part of Kadu encryption module.
  This is a QCA2 implementation of the SIMLite algorithm, used by kadu,
  ekg and other gadu-gadu clients for end-to-end message encryption.
  This is a rewrite of OpenSSL implementation of the algorithm.

 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.  *
 *
 */
#include <QtCore>
#include "kadu_encryption_simlite.h"
#include "pkcs1_certificate.h"

using namespace QCA;

bool KaduEncryptionSIMLite::writePublicKey(const RSAPublicKey &key, const QString &keyId)
{
	QString pubKeyPath;
	QTextStream(&pubKeyPath) << KeysPath << keyId << ".pem";

	//put the key into a PKCS#1 certificate
	SecureArray certificate;
	PKCS1Certificate pkcs1;
	PKCS1Certificate::ConversionStatus status = pkcs1.publicKeyToDER(key, certificate);
	if(status != PKCS1Certificate::OK)
		return false;

	//Base64 encode the certificate
	Base64 encoder;
	encoder.setLineBreaksEnabled(true);
	encoder.setLineBreaksColumn(64);
	SecureArray encodedCert = encoder.encode(certificate);
	if(!encoder.ok())
		return false;

	//write the certificate into a file
	QFile file(pubKeyPath);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	file.write("-----BEGIN RSA PUBLIC KEY-----\n");
	file.write(encodedCert.toByteArray());
	if(file.write("\n-----END RSA PUBLIC KEY-----\n") == -1)
		return false;
	file.close();

	return true;
}

bool KaduEncryptionSIMLite::writePrivateKey(const RSAPrivateKey &key)
{
	QString privKeyPath;
	QTextStream(&privKeyPath) << KeysPath << "private.pem";

	//put the key into a PKCS#1 certificate
	SecureArray certificate;
	PKCS1Certificate pkcs1;
	PKCS1Certificate::ConversionStatus status = pkcs1.privateKeyToDER(key, certificate);
	if(status != PKCS1Certificate::OK)
		return false;

	//Base64 encode the certificate
	Base64 encoder;
	encoder.setLineBreaksEnabled(true);
	encoder.setLineBreaksColumn(64);
	SecureArray encodedCert = encoder.encode(certificate);
	if(!encoder.ok())
		return false;

	//write the certificate into a file
	QFile file(privKeyPath);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	file.write("-----BEGIN RSA PRIVATE KEY-----\n");
	file.write(encodedCert.toByteArray());
	if(file.write("\n-----END RSA PRIVATE KEY-----\n") == -1)
		return false;
	file.close();

	return true;
}

bool KaduEncryptionSIMLite::generateKeys(QString keyId)
{
	//generate the private key
	PrivateKey privateKey = KeyGenerator().createRSA(1024);
	if(privateKey.isNull())
	{
		Error = KEE_GENERATE_PRIVKEY;
		return false;
	}

	//now the public key
	PublicKey publicKey = privateKey.toPublicKey();
	if(!publicKey.canEncrypt())
	{
		Error = KEE_GENERATE_PUBKEY;
		return false;
	}

	//write the keys
	if(!writePrivateKey(static_cast<RSAPrivateKey &>(privateKey)))
	{
		Error = KEE_WRITE_PRIVKEY;
		return false;
	}
	if(!writePublicKey(static_cast<RSAPublicKey &>(publicKey), keyId))
	{
		Error = KEE_WRITE_PUBKEY;
		return false;
	}
	return true;
}

bool KaduEncryptionSIMLite::publicKeyCertificateFromFile(QString &keyId, SecureArray &out)
{
	//construct the path to the key file
	QString keyFilePath;
	QTextStream(&keyFilePath) << KeysPath << keyId << ".pem";

	//open the file containing the certificate
	QFile file(keyFilePath);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	//check if the first line is the RSA 'begin public key' clause
	QString line(file.readLine());
	if(line != "-----BEGIN RSA PUBLIC KEY-----\n")
	{
		file.close();
		return false;
	}
	//for security reasons, read the data straight into a SecureArray
	SecureArray certificate;
	while(!file.atEnd())
	{
		SecureArray tmp(file.readLine());
		//if this was the last line, check if it's the RSA 'end public key' clause
		if(file.atEnd())
		{
			line = tmp.toByteArray();
			if(line != "-----END RSA PUBLIC KEY-----\n" && line != "-----END RSA PUBLIC KEY-----")
			{
				file.close();
				return false;
			}
		}
		else certificate.append(tmp);
	}
	file.close();

	//Base64 decode the certificate
	Base64 decoder(Decode);
	decoder.setLineBreaksEnabled(true);
	out = decoder.decode(certificate);
	out += decoder.final();
	if(!decoder.ok())
		return false;

	return true;
}

bool KaduEncryptionSIMLite::readPublicKey(PublicKey &key, QString &keyId)
{
	SecureArray certificate;
	if(!publicKeyCertificateFromFile(keyId, certificate))
		return false;

	PKCS1Certificate::ConversionStatus status;
	PKCS1Certificate pkcs1;
	key = pkcs1.publicKeyFromDER(certificate, status);
	if(status != PKCS1Certificate::OK)
		return false;
	return true;
}

bool KaduEncryptionSIMLite::privateKeyCertificateFromFile(SecureArray &out)
{
	//construct the path to the key file
	QString keyFilePath;
	QTextStream(&keyFilePath) << KeysPath << "private.pem";

	//open the file containing the certificate
	QFile file(keyFilePath);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	//check if the first line is the RSA 'begin private key' clause
	QString line(file.readLine());
	if(line != "-----BEGIN RSA PRIVATE KEY-----\n")
	{
		file.close();
		return false;
	}
	//for security reasons, read the data straight into a SecureArray
	SecureArray certificate;
	while(!file.atEnd())
	{
		SecureArray tmp(file.readLine());
		//if this was the last line, check if it's the RSA 'end private key' clause
		if(file.atEnd())
		{
			line = tmp.toByteArray();
			if(line != "-----END RSA PRIVATE KEY-----\n" && line != "-----END RSA PRIVATE KEY-----")
			{
				file.close();
				return false;
			}
		}
		else certificate.append(tmp);
	}
	file.close();

	//Base64 decode the certificate
	Base64 decoder(Decode);
	decoder.setLineBreaksEnabled(true);
	decoder.setLineBreaksColumn(64);
	out = decoder.decode(certificate);
	out += decoder.final();
	if(!decoder.ok())
		return false;

	return true;
}

bool KaduEncryptionSIMLite::readPrivateKey(PrivateKey &key)
{
	SecureArray certificate;
	if(!privateKeyCertificateFromFile(certificate))
		return false;

	PKCS1Certificate::ConversionStatus status;
	PKCS1Certificate pkcs1;
	key = pkcs1.privateKeyFromDER(certificate, status);
	if(status != PKCS1Certificate::OK)
		return false;
	return true;
}

bool KaduEncryptionSIMLite::encrypt(QByteArray &message, QString keyId)
{
	//read the desired key
	PublicKey publicKey;
	if(!readPublicKey(publicKey, keyId))
	{
		Error = KEE_READ_PUBKEY;
		return false;
	}
	//check if the key can encrypt
	if(!publicKey.canEncrypt())
	{
		Error = KEE_KEY_CANNOT_ENCRYPT;
		return false;
	}

	//generate a symmetric key for Blowfish (16 bytes in length)
	SymmetricKey blowfishKey(16);
	//encrypt the symmetric key using the RSA public key
	SecureArray encryptedBlowfishKey = publicKey.encrypt(blowfishKey, EME_PKCS1_OAEP);
	if(encryptedBlowfishKey.isEmpty())
	{
		Error = KEE_ENCRYPTION;
		return false;
	}

	//create an initialisation vector (8 zeros)
	char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	InitializationVector iv(QByteArray(ivec, 8));
	//encrypt the message using the Blowfish key:
	//create a 128 bit Blowfish cipher object using Cipher Block Chaining (CBC) mode,
	//with default padding and for encoding
	Cipher cipher(QString("blowfish"), Cipher::CBC, Cipher::DefaultPadding, Encode, blowfishKey, iv);

	//prepare the header
	sim_message_header head;
	memset(&head, 0, sizeof(head));
	//put the magic number into it
	head.magicFirstPart = SIM_MAGIC_V1_1;
	head.magicSecondPart = SIM_MAGIC_V1_2;
	//fill the iv in the header with some random bytes (using IV is a simple way)
	InitializationVector headIV(8);
	memcpy(head.init, headIV.data(), 8);

	//the actual encryption
	message = QByteArray((const char *)&head, sizeof(sim_message_header)) + message;
	SecureArray encrypted = cipher.update(message);
	if(!cipher.ok())
	{
		Error = KEE_ENCRYPTION;
		return false;
	}
	//output the final block
	encrypted.append(cipher.final());
	if(!cipher.ok())
	{
		Error = KEE_ENCRYPTION;
		return false;
	}

	//build the encrypted message
	encrypted = encryptedBlowfishKey + encrypted;
	//encode it using Base64 algorithm
	Base64 encoder;
	encrypted = encoder.encode(encrypted);
//NOTE: this seems to break the message (and without it everything works fine)
//	encrypted += encoder.final();
	if(!encoder.ok()) {
		Error = KEE_ENCRYPTION;
		return false;
	}

	//finally, put the encrypted message into the output QByteArray
	message = encrypted.toByteArray();
	return true;
}

bool KaduEncryptionSIMLite::decrypt(QByteArray &message)
{
	//check if the message has at least the length of the shortest possible encrypted message
	if(message.length() < 192)
	{
		Error = KEE_READ_INVALID_MESSAGE;
		return false;
	}

	//read the private key
	PrivateKey privateKey;
	if(!readPrivateKey(privateKey))
	{
		Error = KEE_READ_PRIVKEY;
		return false;
	}

	//check if the key can decrypt
	if(!privateKey.canDecrypt())
	{
		Error = KEE_KEY_CANNOT_DECRYPT;
		return false;
	}

	//decode the message from the Base64 encoding
	Base64 decoder(Decode);
	SecureArray decodedMessage = decoder.stringToArray(message);
//	decodedMessage += decoder.final();
	if(!decoder.ok()) {
		Error = KEE_DECRYPTION;
		return false;
	}

	//extract the Blowfish key (first 128 characters)
	SecureArray encryptedBlowfishKey(decodedMessage.toByteArray().left(128));
	//and the encrypted message (the rest)
	SecureArray encryptedMessage(decodedMessage.toByteArray().mid(128));

	SymmetricKey blowfishKey;
	if(!privateKey.decrypt(encryptedBlowfishKey, &blowfishKey, EME_PKCS1_OAEP))
	{
		Error = KEE_DECRYPTION;
		return false;
	}

	//recreate the initialization vector (should be the same as the one used for ciphering)
	char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	InitializationVector iv(QByteArray(ivec, 8));
	//now that we have the symmetric Blowfish key, we can decrypt the message;
	//create a 128 bit Blowfish cipher object using Cipher Block Chaining (CBC) mode,
	//with default padding and for decoding
	Cipher cipher(QString("blowfish"), Cipher::CBC, Cipher::DefaultPadding, Decode, blowfishKey, iv);

	//decipher the message (put the message into the decoding cipher object)
	SecureArray plainText = cipher.update(encryptedMessage);
	if(!cipher.ok())
	{
		Error = KEE_DECRYPTION;
		return false;
	}

	//get the last block (with its padding removed)
	plainText.append(cipher.final());
	if(!cipher.ok())
	{
		Error = KEE_DECRYPTION;
		return false;
	}

	//check whether the decrypted data length is at least the size of the header -
	//if not, then we have an invalid message
	if(plainText.size() < (int)sizeof(sim_message_header))
	{
		Error = KEE_READ_INVALID_MESSAGE;
		return false;
	}

	//extract the header from the decrypted data and check if the magic number is
	//correct
	sim_message_header head;
	memcpy(&head, plainText.data(), sizeof(sim_message_header));
	if(head.magicFirstPart != SIM_MAGIC_V1_1 || head.magicSecondPart != SIM_MAGIC_V1_2)
	{
		Error = KEE_READ_INVALID_MESSAGE;
		return false;
	}

	//the message has been decrypted! :D
	//put it into the input/output byte array
	message = &plainText.data()[sizeof(sim_message_header)];
	return true;
}

QString KaduEncryptionSIMLite::calculatePublicKeyFingerprint(QString keyId)
{
	SecureArray certificate;
	if(!publicKeyCertificateFromFile(keyId, certificate))
		return QString();

	//the fingerprint is an SHA1 hash of the key certificate
	Hash sha1Hash("sha1");
	QString fingerprint = arrayToHex(sha1Hash.hash(certificate).toByteArray());
	//the QCA::arrayToHex() function doesn't put colons between bytes, so insert them manually
	return fingerprint.replace(QRegExp("([\\da-fA-F]{2}(?!$))"), "\\1:");
}

const char *KaduEncryptionSIMLite::errorDescription()
{
	switch(Error)
	{
		case KEE_NO_ERROR:
			return "No error";
		case KEE_GENERATE_PUBKEY:
			return "Could not generate public key";
		case KEE_GENERATE_PRIVKEY:
			return "Could not generate private key";
		case KEE_WRITE_PUBKEY:
			return "Could not write public key";
		case KEE_WRITE_PRIVKEY:
			return "Could not write private key";
		case KEE_READ_PUBKEY:
			return "Could not read the public key";
		case KEE_READ_PRIVKEY:
			return "Could not read the private key";
		case KEE_ENCRYPTION:
			return "Error while encrypting";
		case KEE_DECRYPTION:
			return "Error while decrypting";
		case KEE_KEY_CANNOT_ENCRYPT:
			return "The (public) key cannot encrypt";
		case KEE_KEY_CANNOT_DECRYPT:
			return "The (private) key cannot decrypt";
		case KEE_READ_INVALID_MESSAGE:
			return "The received encrypted message is invalid";
	}
	//the function should never reach this point
	return 0;
}

void KaduEncryptionSIMLite::release()
{
	delete this;
}
