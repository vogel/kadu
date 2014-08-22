/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_DECRYPTOR_H
#define ENCRYPTION_NG_SIMLITE_DECRYPTOR_H

#include <QtCore/QObject>
#include <QtCrypto>

#include "accounts/account.h"

#include "plugins/encryption_ng/decryptor.h"
#include "plugins/encryption_ng/keys/key.h"

class EncryptioNgSimliteDecryptor : public Decryptor
{
	Q_OBJECT

	Account MyAccount;
	QCA::PrivateKey DecodingKey;
	bool Valid;

	void updateKey();
	QCA::PrivateKey getPrivateKey(const Key &key);

private slots:
	void keyUpdated(const Key &key);

public:
	EncryptioNgSimliteDecryptor(const Account &account, EncryptionProvider *provider, QObject *parent = 0);
	virtual ~EncryptioNgSimliteDecryptor();

	virtual RawMessage decrypt(const RawMessage &rawMessage, Chat chat, bool *ok) override;

	bool isValid() { return Valid; }

};

#endif // ENCRYPTION_NG_SIMLITE_DECRYPTOR_H
