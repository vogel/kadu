/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "base64.h"

#include "configuration/xml-configuration-file.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"
#include "misc/misc.h"
#include "os/generic/system-info.h"

#include "jabber-protocol.h"

#include "jabber-account-details.h"

JabberAccountDetails::JabberAccountDetails(AccountShared *data) :
		AccountDetails(data), EncryptionMode(JabberAccountDetails::Encryption_Auto)
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
	QString priorityString = loadValue<QString>("Priority");
	setAutoResource(loadValue<bool>("AutoResource"));
	if (resourceString.isEmpty() && !AutoResource)
		resourceString = "Kadu";
	setResource(AutoResource ? SystemInfo::instance()->localHostName() : resourceString);
	bool ok = false;
	int priority = priorityString.toInt(&ok);
	if (!ok)
		priority = 5;
	setPriority(priority);
	setDataTransferProxy(loadValue<QString>("DataTransferProxy"));

	setUseCustomHostPort(loadValue<bool>("UseCustomHostPort"));
	setCustomHost(loadValue<QString>("CustomHost"));
	setCustomPort(loadValue<int>("CustomPort", 5222));

	setEncryptionMode((EncryptionFlag)loadValue<int>("EncryptionMode", 2));
	setPlainAuthMode((AllowPlainType)loadValue<int>("PlainAuthMode", 2));
	setLegacySSLProbe(loadValue<bool>("LegacySSLProbe"));
	setTlsOverrideCert(XMPP::Base64::decode(loadValue<QByteArray>("TlsOverrideCert")));
	setTlsOverrideDomain(loadValue<QString>("TlsOverrideDomain"));

	setSendTypingNotification(loadValue<bool>("SendTypingNotification", true));
	setSendGoneNotification(loadValue<bool>("SendGoneNotification", true));
}

void JabberAccountDetails::store()
{
	if (!isValidStorage())
		return;

	storeValue("AutoResource", autoResource());
	storeValue("Resource", resource());
	storeValue("Priority", priority());
	storeValue("DataTransferProxy", dataTransferProxy());

	storeValue("UseCustomHostPort", useCustomHostPort());
	storeValue("CustomHost", customHost());
	storeValue("CustomPort", customPort());

	storeValue("EncryptionMode", encryptionMode());
	storeValue("PlainAuthMode", plainAuthMode());
	storeValue("LegacySSLProbe", legacySSLProbe());
	storeValue("TlsOverrideCert", XMPP::Base64::encode(tlsOverrideCert()));
	storeValue("TlsOverrideDomain", tlsOverrideDomain());

	storeValue("SendTypingNotification", sendTypingNotification());
	storeValue("SendGoneNotification", sendGoneNotification());
}
