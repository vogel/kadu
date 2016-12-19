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

#include "jabber-account-data.h"

#include "accounts/account-shared.h"
#include "os/generic/system-info.h"

#include <QtCore/QUuid>

JabberAccountData::JabberAccountData(AccountShared *data) :
		m_data{data}
{
}

JabberAccountData::~JabberAccountData()
{
}

int JabberAccountData::priority() const
{
	auto priorityString = m_data->loadValue<QString>("Priority", "5");
	auto ok = false;
	auto priority = priorityString.toInt(&ok);
	return ok
		? priority
		: 5;
}

void JabberAccountData::setPriority(int priority) const
{
	m_data->storeValue("Priority", priority);
	emit m_data->updated();
}

QString JabberAccountData::resource(SystemInfo &systemInfo) const
{
	auto res = m_data->loadValue<QString>("Resource");
	if (res.isEmpty() || res == "Kadu")
	{
		auto guid = QUuid::createUuid().toString();
		res = "Kadu-" + guid.mid(1, guid.length() - 2);
	}
	if (autoResource())
		res = systemInfo.localHostName();
	return res;
}

void JabberAccountData::setResource(const QString &resource) const
{
	m_data->storeValue("Resource", resource);
}

bool JabberAccountData::autoResource() const
{
	return m_data->loadValue<bool>("AutoResource", false);
}

void JabberAccountData::setAutoResource(bool autoResource) const
{
	m_data->storeValue("AutoResource", autoResource);
}

bool JabberAccountData::useCustomHostPort() const
{
	return m_data->loadValue<bool>("UseCustomHostPort", false);
}

void JabberAccountData::setUseCustomHostPort(bool useCustomHostPort) const
{
	m_data->storeValue("UseCustomHostPort", useCustomHostPort);
}

QString JabberAccountData::customHost() const
{
	return m_data->loadValue<QString>("CustomHost");
}

void JabberAccountData::setCustomHost(const QString &customHost) const
{
	m_data->storeValue("CustomHost", customHost);
}

int JabberAccountData::customPort() const
{
	return m_data->loadValue<int>("CustomPort", 5222);
}

void JabberAccountData::setCustomPort(int customPort) const
{
	m_data->storeValue("CustomPort", customPort);
}

JabberAccountData::EncryptionFlag JabberAccountData::encryptionMode() const
{
	return static_cast<EncryptionFlag>(m_data->loadValue<int>("EncryptionMode", static_cast<int>(Encryption_Auto)));
}

void JabberAccountData::setEncryptionMode(EncryptionFlag encryptionMode) const
{
	m_data->storeValue("EncryptionMode", static_cast<int>(encryptionMode));
}

JabberAccountData::AllowPlainType JabberAccountData::plainAuthMode() const
{
	return static_cast<AllowPlainType>(m_data->loadValue<int>("PlainAuthMode", static_cast<int>(AllowPlainOverTLS)));
}

void JabberAccountData::setPlainAuthMode(AllowPlainType plainAuthMode) const
{
	m_data->storeValue("PlainAuthMode", static_cast<int>(plainAuthMode));
}

QString JabberAccountData::tlsOverrideDomain() const
{
	return m_data->loadValue<QString>("TlsOverrideDomain");
}

void JabberAccountData::setTlsOverrideDomain(const QString &tlsOverrideDomain) const
{
	m_data->storeValue("TlsOverrideDomain", tlsOverrideDomain);
}

bool JabberAccountData::sendTypingNotification() const
{
	return m_data->loadValue<bool>("SendTypingNotification", true);
}

void JabberAccountData::setSendTypingNotification(bool sendTypingNotification) const
{
	m_data->storeValue("SendTypingNotification", sendTypingNotification);
}

bool JabberAccountData::sendGoneNotification() const
{
	return m_data->loadValue<bool>("SendGoneNotification", true);
}

void JabberAccountData::setSendGoneNotification(bool sendGoneNotification) const
{
	m_data->storeValue("SendGoneNotification", sendGoneNotification);
}

bool JabberAccountData::publishSystemInfo() const
{
	return m_data->loadValue<bool>("PublishSystemInfo", true);
}

void JabberAccountData::setPublishSystemInfo(bool publishSystemInfo) const
{
	m_data->storeValue("PublishSystemInfo", publishSystemInfo);
}

QString JabberAccountData::dataTransferProxy() const
{
	return m_data->loadValue<QString>("DataTransferProxy");
}

void JabberAccountData::setDataTransferProxy(const QString &dataTransferProxy)
{
	m_data->storeValue("DataTransferProxy", dataTransferProxy);
	emit m_data->updated();
}

bool JabberAccountData::requireDataTransferProxy() const
{
	return m_data->loadValue<bool>("RequireDataTransferProxy", false);
}

void JabberAccountData::setRequireDataTransferProxy(bool requireDataTransferProxy)
{
	m_data->storeValue("RequireDataTransferProxy", requireDataTransferProxy);
	emit m_data->updated();
}
