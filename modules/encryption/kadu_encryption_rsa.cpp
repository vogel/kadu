/*
  File name: kadu_encryption_rsa.cpp
  Copyright: (C) 2007, 2008-2009 Tomasz Kazmierczak

  Creation date: 2007-10-25
  Last modification date: 2009-03-08

  This file is part of Kadu encryption module

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
#include "kadu_encryption_rsa.h"

using namespace QCA;

bool KaduEncryptionRSA::generateKeys(QString keyId)
{
	//generate the private key
	PrivateKey privkey = KeyGenerator().createRSA(1024);
	if(privkey.isNull())
	{
		Error = KEE_GENERATE_PRIVKEY;
		return false;
	}

	//now the public key
	PublicKey pubkey = privkey.toPublicKey();
	if(!pubkey.canEncrypt())
	{
		Error = KEE_GENERATE_PUBKEY;
		return false;
	}

	//write the keys to pem files
	QString privKeyPath;
	QTextStream(&privKeyPath) << KeysPath << "rsa_private.pem";
	if(!privkey.toPEMFile(privKeyPath, SecureArray()))
	{
		Error = KEE_WRITE_PRIVKEY;
		return false;
	}
	QString pubKeyPath;
	QTextStream(&pubKeyPath) << KeysPath << "rsa_" << keyId << ".pem";
	if(not pubkey.toPEMFile(pubKeyPath))
	{
		Error = KEE_WRITE_PUBKEY;
		return false;
	}
	return true;
}

bool KaduEncryptionRSA::readPubKey(PublicKey &key, QString &keyId)
{
	ConvertResult result;
	QString pubKeyPath;
	QTextStream(&pubKeyPath) << KeysPath << "rsa_" << keyId << ".pem";
	key = PublicKey::fromPEMFile(pubKeyPath, &result);
	if(ConvertGood != result)
		return false;
	return true;
}

bool KaduEncryptionRSA::readPrivKey(PrivateKey &key)
{
	ConvertResult result;
	QString privKeyPath;
	QTextStream(&privKeyPath) << KeysPath << "rsa_private.pem";
	key = PrivateKey::fromPEMFile(privKeyPath, SecureArray(), &result);
	if(ConvertGood != result)
		return false;
	return true;
}

bool KaduEncryptionRSA::encrypt(QByteArray &message, QString keyId)
{
	//read the desired key
	PublicKey pubkey;
	if(!readPubKey(pubkey, keyId))
	{
		Error = KEE_READ_PUBKEY;
		return false;
	}
	//check if the key can encrypt
	if(!pubkey.canEncrypt())
	{
		Error = KEE_KEY_CANNOT_ENCRYPT;
		return false;
	}

	//put the message into a SecureArray;
	SecureArray text(message);
	//encrypt the message
	SecureArray result = pubkey.encrypt(text, EME_PKCS1_OAEP);
	if(result.isEmpty())
	{
		Error = KEE_ENCRYPTION;
		return false;
	}
	//encode the data using Base64 algorithm
	//the default constructor of QCA::Base64 is equivalent to QCA::Base64(QCA::Encode)
	Base64 encoder;
	message = encoder.encode(result).toByteArray();
	return true;
}

bool KaduEncryptionRSA::decrypt(QByteArray &message)
{
	//read the private key
	PrivateKey privkey;
	if(!readPrivKey(privkey))
	{
		Error = KEE_READ_PRIVKEY;
		return false;
	}
	//check if the key can decrypt
	if(!privkey.canDecrypt())
	{
		Error = KEE_KEY_CANNOT_DECRYPT;
		return false;
	}

	//decode the string (from Base64) and put it into a SecureArray
	Base64 decoder(Decode);
	SecureArray encrypted(decoder.decode(SecureArray(message))), decrypted;
	//decrypt
	if(!privkey.decrypt(encrypted, &decrypted, EME_PKCS1_OAEP))
	{
		Error = KEE_DECRYPTION;
		return false;
	}

	//put the decrypted data into the message string
	message = decrypted.data();
	return true;
}

QString KaduEncryptionRSA::calculatePublicKeyFingerprint(QString keyId)
{
	//read the desired key
	PublicKey pubkey;
	if(!readPubKey(pubkey, keyId))
		return QString();

	//the fingerprint is an SHA1 hash of the key certificate
	Hash sha1Hash("sha1");
	QString fingerprint = arrayToHex(sha1Hash.hash(pubkey.toDER()).toByteArray());
	//the QCA::arrayToHex() function doesn't put colons between bytes, so insert them manually
	return fingerprint.replace(QRegExp("([\\da-fA-F]{2}(?!$))"), "\\1:");
}

const char *KaduEncryptionRSA::errorDescription()
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

void KaduEncryptionRSA::release()
{
	delete this;
}
