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

#include "decryptor-wrapper.h"

void DecryptorWrapper::addDecryptor(Decryptor *decryptor)
{
	Decryptors.append(decryptor);
	connect(decryptor, SIGNAL(destroyed(QObject*)), this, SLOT(decryptorDestroyed(QObject*)));
}

void DecryptorWrapper::removeDecryptor(Decryptor* decryptor)
{
	Decryptors.removeAll(decryptor);
	disconnect(decryptor, SIGNAL(destroyed(QObject*)), this, SLOT(decryptorDestroyed(QObject*)));
}

void DecryptorWrapper::decryptorDestroyed(QObject *decryptor)
{
	Decryptors.removeAll(dynamic_cast<Decryptor *>(decryptor));
}

QByteArray DecryptorWrapper::decrypt(const QByteArray &data)
{
	QByteArray decrypted = data;

	foreach (Decryptor *decryptor, Decryptors)
		decrypted = decryptor->decrypt(decrypted);

	return decrypted;
}
