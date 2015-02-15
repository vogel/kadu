/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_KEY_IMPORTER_H
#define ENCRYPTION_NG_SIMLITE_KEY_IMPORTER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"

class QFileInfo;

class EncryptioNgSimliteKeyImporter : public QObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EncryptioNgSimliteKeyImporter)

	static EncryptioNgSimliteKeyImporter *Instance;

	EncryptioNgSimliteKeyImporter();

	void importKey(const Account &account, const QFileInfo &fileInfo);
	void importKeys(const Account &account);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static void createInstance();
	static void destroyInstance();

};

#endif // ENCRYPTION_NG_SIMLITE_KEY_IMPORTER_H
