/*
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

#ifndef ENCRYPTION_NG_SIMLITE_KEY_GENERATOR_H
#define ENCRYPTION_NG_SIMLITE_KEY_GENERATOR_H

#include <QtCore/QtGlobal>
#include <QtCrypto>

#include "plugins/encryption_ng/key-generator.h"

class EncryptioNgSimliteKeyGenerator : public KeyGenerator
{
	Q_DISABLE_COPY(EncryptioNgSimliteKeyGenerator)

	static EncryptioNgSimliteKeyGenerator *Instance;

	EncryptioNgSimliteKeyGenerator() {}
	virtual ~EncryptioNgSimliteKeyGenerator() {}

	QCA::SecureArray writePublicKey(const QCA::RSAPublicKey &key);
	QCA::SecureArray writePrivateKey(const QCA::RSAPrivateKey &key);

public:
	static void createInstance();
	static void destroyInstance();

	static EncryptioNgSimliteKeyGenerator * instance() { return Instance; }

	virtual bool hasKeys(const Account &account);
	virtual bool generateKeys(const Account &account);

};

#endif // ENCRYPTION_NG_SIMLITE_KEY_GENERATOR_H
