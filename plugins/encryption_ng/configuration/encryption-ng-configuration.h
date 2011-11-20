/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_CONFIGURATION_H
#define ENCRYPTION_NG_CONFIGURATION_H

#include <QtCore/QString>

#include "configuration/configuration-aware-object.h"

class EncryptionNgConfiguration : private ConfigurationAwareObject
{
	Q_DISABLE_COPY(EncryptionNgConfiguration)

	static EncryptionNgConfiguration * Instance;

	bool EncryptByDefault;
	bool EncryptAfterReceiveEncryptedMessage;

	EncryptionNgConfiguration();
	virtual ~EncryptionNgConfiguration();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	static EncryptionNgConfiguration * instance();

	static void createInstance();
	static void destroyInstance();

	bool encryptByDefault() { return EncryptByDefault; }
	bool encryptAfterReceiveEncryptedMessage() { return EncryptAfterReceiveEncryptedMessage; }

};

#endif // ENCRYPTION_NG_CONFIGURATION_H
