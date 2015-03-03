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

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"
#include "misc/misc.h"
#include "os/generic/system-info.h"

#include "jabber-protocol.h"

#include "jabber-account-details.h"

JabberAccountDetails::JabberAccountDetails(AccountShared *data) :
		AccountDetails(data), AutoResource(false), Priority{100}, UseCustomHostPort(false), CustomPort(5222),
		EncryptionMode(Encryption_Auto), PlainAuthMode(AllowPlainOverTLS), RequireDataTransferProxy{false},
		SendTypingNotification(true), SendGoneNotification(true), PublishSystemInfo(true)
{
	OpenChatRunner = new JabberOpenChatWithRunner(data);
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
}

JabberAccountDetails::~JabberAccountDetails()
{
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	delete OpenChatRunner;
	OpenChatRunner = 0;
}

void JabberAccountDetails::load()
{
	if (!isValidStorage())
		return;

	AccountDetails::load();

	QString resourceString = loadValue<QString>("Resource");
	QString priorityString = loadValue<QString>("Priority", "100");
	AutoResource = loadValue<bool>("AutoResource", false);

	if (resourceString.isEmpty() || resourceString == "Kadu")
	{
		auto guid = QUuid::createUuid().toString();
		resourceString = "Kadu-" + guid.mid(1, guid.length() - 2);
	}

	Resource = AutoResource ? SystemInfo::instance()->localHostName() : resourceString;
	bool ok = false;
	int priority = priorityString.toInt(&ok);
	if (!ok)
		priority = 5;
	Priority = priority;
	DataTransferProxy = loadValue<QString>("DataTransferProxy");
	RequireDataTransferProxy = loadValue<bool>("RequireDataTransferProxy", false);

	UseCustomHostPort = loadValue<bool>("UseCustomHostPort", false);
	CustomHost = loadValue<QString>("CustomHost");
	CustomPort = loadValue<int>("CustomPort", 5222);

	EncryptionMode = (EncryptionFlag)loadValue<int>("EncryptionMode", (int)Encryption_Auto);
	PlainAuthMode = (AllowPlainType)loadValue<int>("PlainAuthMode", (int)AllowPlainOverTLS);
	// TlsOverrideCert = Base64::decode(loadValue<QByteArray>("TlsOverrideCert"));
	TlsOverrideDomain = loadValue<QString>("TlsOverrideDomain");

	SendTypingNotification = loadValue<bool>("SendTypingNotification", true);
	SendGoneNotification = loadValue<bool>("SendGoneNotification", true);
	PublishSystemInfo = loadValue<bool>("PublishSystemInfo", true);
}

void JabberAccountDetails::store()
{
	if (!isValidStorage())
		return;

	storeValue("AutoResource", AutoResource);
	storeValue("Resource", Resource);
	storeValue("Priority", Priority);
	storeValue("DataTransferProxy", DataTransferProxy);
	storeValue("RequireDataTransferProxy", RequireDataTransferProxy);

	storeValue("UseCustomHostPort", UseCustomHostPort);
	storeValue("CustomHost", CustomHost);
	storeValue("CustomPort", CustomPort);

	storeValue("EncryptionMode", EncryptionMode);
	storeValue("PlainAuthMode", PlainAuthMode);
	// storeValue("TlsOverrideCert", Base64::encode(TlsOverrideCert).toAscii());
	storeValue("TlsOverrideDomain", TlsOverrideDomain);

	storeValue("SendTypingNotification", SendTypingNotification);
	storeValue("SendGoneNotification", SendGoneNotification);
	storeValue("PublishSystemInfo", PublishSystemInfo);
}

QString JabberAccountDetails::dataTransferProxy()
{
	ensureLoaded();
	return DataTransferProxy;
}

void JabberAccountDetails::setDataTransferProxy(const QString &dataTransferProxy)
{
	ensureLoaded();
	DataTransferProxy = dataTransferProxy;

	emit dataTransferProxyChanged();
}

bool JabberAccountDetails::requireDataTransferProxy()
{
	ensureLoaded();
	return RequireDataTransferProxy;
}

void JabberAccountDetails::setRequireDataTransferProxy(bool requireDataTransferProxy)
{
	ensureLoaded();
	RequireDataTransferProxy = requireDataTransferProxy;

	emit dataTransferProxyChanged();
}

int JabberAccountDetails::priority()
{
	ensureLoaded();
	return Priority;
}

void JabberAccountDetails::setPriority(int priority)
{
	ensureLoaded();
	Priority = priority;

	emit priorityChanged();
}

#include "moc_jabber-account-details.cpp"
