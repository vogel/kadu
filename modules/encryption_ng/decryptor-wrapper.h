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

#ifndef DECRYPTOR_WRAPPER_H
#define DECRYPTOR_WRAPPER_H

#include "decryptor.h"

class DecryptorWrapper : public Decryptor
{
	Q_OBJECT

	QList<Decryptor *> Decryptors;

private slots:
	void decryptorDestroyed(QObject *decryptor);

public:
	virtual ~DecryptorWrapper() {}

	virtual QByteArray decrypt(const QByteArray &data);

	void addDecryptor(Decryptor *decryptor);
	void removeDecryptor(Decryptor *decryptor);

};

#endif // DECRYPTOR_WRAPPER_H
