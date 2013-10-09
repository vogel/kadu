/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_CONFIGURATION_H
#define ENCRYPTION_NG_SIMLITE_CONFIGURATION_H

#include <QtCore/QString>

#include "configuration/configuration-aware-object.h"

class EncryptionNgSimliteConfiguration : private ConfigurationAwareObject
{
	Q_DISABLE_COPY(EncryptionNgSimliteConfiguration)

	static EncryptionNgSimliteConfiguration * Instance;

	bool EncryptAfterReceiveEncryptedMessage;

	EncryptionNgSimliteConfiguration();
	virtual ~EncryptionNgSimliteConfiguration();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	static EncryptionNgSimliteConfiguration * instance();

	static void createInstance();
	static void destroyInstance();

	bool encryptAfterReceiveEncryptedMessage() { return EncryptAfterReceiveEncryptedMessage; }

};

#endif // ENCRYPTION_NG_SIMLITE_CONFIGURATION_H
