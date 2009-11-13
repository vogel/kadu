/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "buddies/ignored-helper.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-shared.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"
#include "gadu-protocol-factory.h"

#include "gadu-importer.h"

GaduImporter * GaduImporter::Instance;

GaduImporter * GaduImporter::instance()
{
	if (0 == Instance)
		Instance = new GaduImporter();

	return Instance;
}

void GaduImporter::importAccounts()
{
	if (0 == config_file.readNumEntry("General", "UIN"))
		return;
	
	Account defaultGaduGadu;
	GaduAccountDetails *accountDetails = new GaduAccountDetails(defaultGaduGadu.storage(), defaultGaduGadu);
	defaultGaduGadu.setDetails(accountDetails);

	defaultGaduGadu.setName("Gadu-Gadu");
	defaultGaduGadu.setId(config_file.readEntry("General", "UIN"));
	defaultGaduGadu.setPassword(unicode2cp(pwHash(config_file.readEntry("General", "Password"))));
	defaultGaduGadu.setRememberPassword(true);
	accountDetails->setAllowDcc(config_file.readBoolEntry("Network", "AllowDCC"));

	QHostAddress host;
	if (!host.setAddress(config_file.readEntry("Network", "DccIP")))
		host.setAddress("0.0.0.0");
	accountDetails->setDccIP(host);
	if (!host.setAddress(config_file.readEntry("Network", "ExternalIP")))
		host.setAddress("0.0.0.0");
	accountDetails->setDccExternalIP(host);

	accountDetails->setDccExternalPort(config_file.readNumEntry("Network", "ExternalPort"));
	accountDetails->setDccPort(config_file.readNumEntry("Network", "ExternalPort"));
	accountDetails->setDccIpDetect(config_file.readBoolEntry("Network", "DccIpDetect"));
	accountDetails->setDccLocalPort(config_file.readNumEntry("Network", "LocalPort"));
	accountDetails->setDccForwarding(config_file.readBoolEntry("Network", "DccForwarding"));
	accountDetails->setRemoveCompletedTransfers(config_file.readBoolEntry("Network", "RemoveCompletedTransfers"));

	defaultGaduGadu.setUseProxy(config_file.readBoolEntry("Network", "UseProxy"));
	if (!host.setAddress(config_file.readEntry("Network", "ProxyHost")))
		host.setAddress("0.0.0.0");
	defaultGaduGadu.setProxyHost(host);
	defaultGaduGadu.setProxyPassword(config_file.readEntry("Network", "ProxyPassword"));
	defaultGaduGadu.setProxyPort(config_file.readNumEntry("Network", "ProxyPort"));
	defaultGaduGadu.setProxyUser(config_file.readEntry("Network", "ProxyUser"));
	defaultGaduGadu.setProxyRequiresAuthentication(!defaultGaduGadu.proxyUser().isEmpty());

	accountDetails->import_0_6_5_LastStatus();

	AccountManager::instance()->registerAccount(defaultGaduGadu);
}

void GaduImporter::importContacts()
{
	connect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));

	foreach (Buddy buddy, BuddyManager::instance()->buddies())
		buddyAdded(buddy);

	importIgnored();
}

void GaduImporter::importGaduContact(Buddy &buddy)
{
	Account account = AccountManager::instance()->defaultAccount();
	QString id = buddy.customData()["uin"];

	Contact contact;
	contact.setDetails(new GaduContactDetails(contact.storage(), contact));
	contact.setContactAccount(account);
	contact.setOwnerBuddy(buddy);
	contact.setId(id);
	contact.data()->setLoaded(true);
	contact.setBlocked(QVariant(buddy.customData()["blocking"]).toBool());
	contact.setOfflineTo(QVariant(buddy.customData()["offline_to"]).toBool());

	buddy.customData().remove("uin");
	buddy.customData().remove("blocking");
	buddy.customData().remove("offline_to");

	buddy.addContact(contact);

	ContactManager::instance()->addContact(contact);
}

void GaduImporter::importIgnored()
{
	Account account = AccountManager::instance()->defaultAccount();
	if (account.isNull())
		return;

	QDomElement ignored = xml_config_file->getNode("Ignored", XmlConfigFile::ModeFind);
	if (ignored.isNull())
		return;

	QList<QDomElement> ignoredGroups = xml_config_file->getNodes(ignored, "IgnoredGroup");
	foreach (QDomElement ignoredGroup, ignoredGroups)
	{
		BuddySet ignoredList;
		QList<QDomElement> ignoredContacts = xml_config_file->getNodes(ignoredGroup, "IgnoredContact");
		foreach (QDomElement ignoredContact, ignoredContacts)
			ignoredList.insert(BuddyManager::instance()->byId(account, ignoredContact.attribute("uin")));

		if (0 == ignoredList.count())
			continue;

		IgnoredHelper::setIgnored(ignoredList);
	}
}

void GaduImporter::buddyAdded(Buddy &buddy)
{
	if (buddy.customData().contains("uin"))
		importGaduContact(buddy);
}
