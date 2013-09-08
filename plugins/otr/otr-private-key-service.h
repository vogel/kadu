/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_PRIVATE_KEY_SERVICE_H
#define OTR_PRIVATE_KEY_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QObject>

class Account;

class OtrCreatePrivateKeyJob;
class OtrUserState;

class OtrPrivateKeyService : public QObject
{
	Q_OBJECT

	OtrUserState *UserState;
	QMap<Account, OtrCreatePrivateKeyJob *> CreateJobs;

	QString privateStoreFileName();

private slots:
	void jobFinished(const Account &account, bool ok);

public:
	explicit OtrPrivateKeyService(QObject *parent = 0);
	virtual ~OtrPrivateKeyService();

	void setUserState(OtrUserState *userState);

	void createPrivateKey(const Account &account);
	void readPrivateKeys();

signals:
	void createPrivateKeyStarted(const Account &account);
	void createPrivateKeyFinished(const Account &account, bool ok);

};


#endif // OTR_PRIVATE_KEY_SERVICE_H
