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

#ifndef OTR_CREAETE_PRIVATE_KEY_JOB_H
#define OTR_CREAETE_PRIVATE_KEY_JOB_H

#include <QtCore/QObject>

class Account;
class OtrUserState;

class OtrCreatePrivateKeyJob : public QObject
{
	Q_OBJECT

	OtrUserState *UserState;
	QString PrivateStoreFileName;

public:
	explicit OtrCreatePrivateKeyJob(QObject *parent = 0);
	virtual ~OtrCreatePrivateKeyJob();

	void setUserState(OtrUserState *userState);
	void setPrivateStoreFileName(const QString &privateStoreFileName);
	void createPrivateKey(const Account &account);

signals:
	void finished(bool ok);

};

#endif // OTR_CREAETE_PRIVATE_KEY_JOB_H
