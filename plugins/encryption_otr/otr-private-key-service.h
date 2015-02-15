/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;

class OtrCreatePrivateKeyJob;
class OtrPathService;
class OtrUserStateService;

class OtrPrivateKeyService : public QObject
{
	Q_OBJECT

public:
	static void wrapperOtrCreatePrivateKey(void *data, const char *accountName, const char *protocol);

	Q_INVOKABLE OtrPrivateKeyService();
	virtual ~OtrPrivateKeyService();

	void createPrivateKey(const Account &account);
	void readPrivateKeys();

signals:
	void createPrivateKeyStarted(const Account &account);
	void createPrivateKeyFinished(const Account &account, bool ok);

private slots:
	INJEQT_SETTER void setPathService(OtrPathService *pathService);
	INJEQT_SETTER void setUserStateService(OtrUserStateService *userStateService);

	void jobFinished(const Account &account, bool ok);

private:
	QPointer<OtrPathService> PathService;
	QPointer<OtrUserStateService> UserStateService;
	QMap<Account, OtrCreatePrivateKeyJob *> CreateJobs;

	QString privateStoreFileName() const;

};
