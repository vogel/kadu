/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class AccountShared;
class SystemInfo;

class QString;

class JabberAccountData final
{

public:
	enum EncryptionFlag
	{
		Encryption_No = 0,
		Encryption_Yes = 1,
		Encryption_Auto = 2,
		Encryption_Legacy = 3
	};

	enum AllowPlainType
	{
		NoAllowPlain = 0,
		AllowPlain = 1,
		AllowPlainOverTLS = 2
	};

	explicit JabberAccountData(AccountShared *data);
	~JabberAccountData();

	int priority() const;
	void setPriority(int priority) const;

	QString resource(SystemInfo &systemInfo) const;
	void setResource(const QString &resource) const;

	bool autoResource() const;
	void setAutoResource(bool autoResource) const;

	bool useCustomHostPort() const;
	void setUseCustomHostPort(bool useCustomHostPort) const;

	QString customHost() const;
	void setCustomHost(const QString &customHost) const;

	int customPort() const;
	void setCustomPort(int customPort) const;

	EncryptionFlag encryptionMode() const;
	void setEncryptionMode(EncryptionFlag encryptionMode) const;

	AllowPlainType plainAuthMode() const;
	void setPlainAuthMode(AllowPlainType plainAuthMode) const;

	QString tlsOverrideDomain() const;
	void setTlsOverrideDomain(const QString &tlsOverrideDomain) const;

	bool sendTypingNotification() const;
	void setSendTypingNotification(bool sendTypingNotification) const;

	bool sendGoneNotification() const;
	void setSendGoneNotification(bool sendGoneNotification) const;

	bool publishSystemInfo() const;
	void setPublishSystemInfo(bool publishSystemInfo) const;

	QString dataTransferProxy() const;
	void setDataTransferProxy(const QString &dataTransferProxy);

	bool requireDataTransferProxy() const;
	void setRequireDataTransferProxy(bool requireDataTransferProxy);

private:
	AccountShared *m_data;

};
