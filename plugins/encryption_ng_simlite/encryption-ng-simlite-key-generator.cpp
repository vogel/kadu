/*
 * Copyright 2007, 2008, 2009 Tomasz Kazmierczak
 * %kadu copyright begin%
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "contacts/contact.h"

#include "plugins/encryption_ng/keys/key.h"
#include "plugins/encryption_ng/keys/keys-manager.h"

#include "pkcs1_certificate.h"

#include "encryption-ng-simlite-key-generator.h"

EncryptioNgSimliteKeyGenerator *EncryptioNgSimliteKeyGenerator::Instance = 0;

void EncryptioNgSimliteKeyGenerator::createInstance()
{
	if (!Instance)
		Instance = new EncryptioNgSimliteKeyGenerator();
}

void EncryptioNgSimliteKeyGenerator::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

bool EncryptioNgSimliteKeyGenerator::hasKeys(const Account &account)
{
	Key key = KeysManager::instance()->byContactAndType(account.accountContact(), "simlite", ActionReturnNull);
	if (key)
		return true;

	key = KeysManager::instance()->byContactAndType(account.accountContact(), "simlite_private", ActionReturnNull);
	if (key)
		return true;

	return false;
}

QCA::SecureArray EncryptioNgSimliteKeyGenerator::writePublicKey(const QCA::RSAPublicKey &key)
{
	QCA::SecureArray result;

	//put the key into a PKCS#1 certificate
	QCA::SecureArray certificate;
	PKCS1Certificate pkcs1;
	PKCS1Certificate::ConversionStatus status = pkcs1.publicKeyToDER(key, certificate);
	if (status != PKCS1Certificate::OK)
		return result;

	//Base64 encode the certificate
	QCA::Base64 encoder;
	encoder.setLineBreaksEnabled(true);
	encoder.setLineBreaksColumn(64);
	QCA::SecureArray encodedCert = encoder.encode(certificate);
	if (!encoder.ok())
		return result;

	result.append("-----BEGIN RSA PUBLIC KEY-----\n");
	result.append(encodedCert);
	result.append("\n-----END RSA PUBLIC KEY-----\n");

	return result;
}

QCA::SecureArray EncryptioNgSimliteKeyGenerator::writePrivateKey(const QCA::RSAPrivateKey &key)
{
	QCA::SecureArray result;

	//put the key into a PKCS#1 certificate
	QCA::SecureArray certificate;
	PKCS1Certificate pkcs1;
	PKCS1Certificate::ConversionStatus status = pkcs1.privateKeyToDER(key, certificate);
	if (status != PKCS1Certificate::OK)
		return result;

	//Base64 encode the certificate
	QCA::Base64 encoder;
	encoder.setLineBreaksEnabled(true);
	encoder.setLineBreaksColumn(64);
	QCA::SecureArray encodedCert = encoder.encode(certificate);
	if (!encoder.ok())
		return result;

	result.append("-----BEGIN RSA PRIVATE KEY-----\n");
	result.append(encodedCert);
	result.append("\n-----END RSA PRIVATE KEY-----\n");

	return result;
}

bool EncryptioNgSimliteKeyGenerator::generateKeys(const Account &account)
{
	QCA::PrivateKey privateKey = QCA::KeyGenerator().createRSA(1024);
	if (privateKey.isNull())
		return false;

	QCA::PublicKey publicKey = privateKey.toPublicKey();
	if (!publicKey.canEncrypt())
		return false;

	QCA::SecureArray storedPrivateKey = writePrivateKey(static_cast<QCA::RSAPrivateKey &>(privateKey));
	QCA::SecureArray storedPublicKey = writePublicKey(static_cast<QCA::RSAPublicKey &>(publicKey ));

	if (storedPrivateKey.isEmpty() || storedPublicKey.isEmpty())
		return false;

	KeysManager::instance()->byContactAndType(account.accountContact(), "simlite_private", ActionCreateAndAdd).setKey(storedPrivateKey);
	KeysManager::instance()->byContactAndType(account.accountContact(), "simlite", ActionCreateAndAdd).setKey(storedPublicKey);

	return true;
}
