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

#include "jabber-account.h"
#include "jabber_protocol.h"
#include "system-info.h"

JabberAccount::JabberAccount(const QUuid &uuid)
	: Account(uuid), EncryptionMode(JabberAccount::Encryption_Auto)
{
}

JabberAccount::~JabberAccount()
{
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
	setCustomHost(loadValue<QString>("CustomHost"));
	setCustomPort(loadValue<int>("CustomPort"));
	setEncryptionMode((EncryptionFlag)loadValue<int>("EncryptionMode"));
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
	storeValue("CustomHost", customHost());
	storeValue("CustomPort", customPort());
	storeValue("EncryptionMode", encryptionMode());
	storeValue("TlsOverrideCert", XMPP::Base64::encode(tlsOverrideCert()));
	storeValue("TlsOverrideDomain", tlsOverrideDomain());
}
