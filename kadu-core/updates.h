/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/accounts-aware-object.h"
#include "protocols/protocol.h"

#include <QtCore/QDateTime>
#include <injeqt/injeqt.h>

class AccountManager;
class ConfigurationManager;
class Configuration;

class QNetworkReply;

class Updates : public QObject, AccountsAwareObject
{
	Q_OBJECT

public:
	explicit Updates(QObject *parent = nullptr);
	virtual ~Updates();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<Configuration> m_configuration;

	bool UpdateChecked;
	QString Query;

	void buildQuery();

	static bool isNewerVersionThan(const QString &version);
	static QString stripVersion(const QString &version);

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();

	void gotUpdatesInfo(QNetworkReply *reply);
	void run();

};
