/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/xml-configuration-file.h"
#include "misc/misc.h"

#include "jabber-account.h"
#include "jabber_protocol.h"
#include "jid-util.h"
#include "system-info.h"

JabberAccount * JabberAccount::loadFromStorage(StoragePoint *storagePoint)
{
	if (!storagePoint || !storagePoint->storage())
		return 0;

	JabberAccount *result = new JabberAccount();
	result->setStorage(storagePoint);
	result->load();

	return result;
}

JabberAccount::JabberAccount(const QUuid &uuid)
	: Account(uuid), EncryptionMode(JabberAccount::Encryption_Auto), IgnoreTLSWarnings(false)
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
	setIgnoreTLSWarnings(loadValue<bool>("IgnoreTLSWarnings"));
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
	storeValue("IgnoreTLSWarnings", ignoreTLSWarnings());
}
