/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "base64.h"

#include "configuration/xml-configuration-file.h"
#include "misc/misc.h"
#include "gui/windows/open-chat-with/open-chat-with-runner-manager.h"

#include "jabber-account.h"
#include "jabber-protocol.h"
#include "utils/system-info.h"

JabberAccount::JabberAccount(const QUuid &uuid)
	: Account(uuid), EncryptionMode(JabberAccount::Encryption_Auto)
{
	OpenChatRunner = new JabberOpenChatWithRunner(this);
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);
}

JabberAccount::~JabberAccount()
{
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	delete OpenChatRunner;
	OpenChatRunner = 0;
}

bool JabberAccount::setId(const QString &id)
{
	if (!Account::setId(id))
		return false;

    	Jid = id;
    	return true;
}
/*
bool JabberAccount::validateId(const QString &id)
{
	XMPP::Jid newJid(JIDUtil::accountFromString(id));
	if (newJid.node().isEmpty() || newJid.domain().isEmpty())
		return false;
	return true;
}
*/
void JabberAccount::load()
{
	if (!isValidStorage())
		return;
	Account::load();

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

	setUseCustomHostPort(loadValue<bool>("UseCustomHostPort"));
	setCustomHost(loadValue<QString>("CustomHost"));
	setCustomPort(loadValue<int>("CustomPort"));

	setEncryptionMode((EncryptionFlag)loadValue<int>("EncryptionMode"));
	setLegacySSLProbe(loadValue<bool>("LegacySSLProbe"));
	setTlsOverrideCert(XMPP::Base64::decode(loadValue<QByteArray>("TlsOverrideCert")));
	setTlsOverrideDomain(loadValue<QString>("TlsOverrideDomain"));
}

void JabberAccount::store()
{
	if (!isValidStorage())
		return;

	Account::store();

	storeValue("AutoResource", autoResource());
	storeValue("Resource", resource());
	storeValue("Priority", priority());

	storeValue("UseCustomHostPort", useCustomHostPort());
	storeValue("CustomHost", customHost());
	storeValue("CustomPort", customPort());

	storeValue("EncryptionMode", encryptionMode());
	storeValue("LegacySSLProbe", legacySSLProbe());
	storeValue("TlsOverrideCert", XMPP::Base64::encode(tlsOverrideCert()));
	storeValue("TlsOverrideDomain", tlsOverrideDomain());
}
