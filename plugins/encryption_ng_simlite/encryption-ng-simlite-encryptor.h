/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_ENCRYPTOR_H
#define ENCRYPTION_NG_SIMLITE_ENCRYPTOR_H

#include <QtCore/QObject>
#include <QtCrypto>

#include "contacts/contact.h"

#include "plugins/encryption_ng/encryptor.h"

class Key;

class EncryptioNgSimliteEncryptor : public Encryptor
{
	Q_OBJECT

	Contact MyContact;
	QCA::PublicKey EncodingKey;
	bool Valid;

	void updateKey();
	QCA::PublicKey getPublicKey(const Key &key);

private slots:
	void keyUpdated(const Key &key);

public:
	EncryptioNgSimliteEncryptor(const Contact &contact, EncryptionProvider *provider, QObject *parent = 0);
	virtual ~EncryptioNgSimliteEncryptor();

	virtual RawMessage encrypt(const RawMessage &rawMessage) override;

	bool isValid() { return Valid; }

};

#endif // ENCRYPTION_NG_SIMLITE_ENCRYPTOR_H
