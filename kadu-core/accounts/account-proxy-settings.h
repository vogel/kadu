/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef ACCOUNT_PROXY_SETTINGS_H
#define ACCOUNT_PROXY_SETTINGS_H

#include <QtCore/QString>
#include <QtNetwork/QHostAddress>
#include <exports.h>

class KADUAPI AccountProxySettings
{
	bool Enabled;
	QString Address;
	int Port;

	bool RequiresAuthentication;
	QString User;
	QString Password;

public:
	AccountProxySettings();
	AccountProxySettings(const AccountProxySettings &copyMe);

	AccountProxySettings & operator = (const AccountProxySettings &copyMe);
	bool operator != (const AccountProxySettings &compare);

	void setEnabled(bool enabled);
	bool enabled();

	void setAddress(const QString &address);
	QString address();

	void setPort(int port);
	int port();

	void setRequiresAuthentication(bool requiresAuthentication);
	bool requiresAuthentication();

	void setUser(const QString &user);
	QString user();

	void setPassword(const QString &password);
	QString password();

};

#endif // ACCOUNT_PROXY_SETTINGS_H
