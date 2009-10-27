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
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "contacts/ignored-helper.h"
#include "contacts/account-data/contact-account-data-manager.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"
#include "gadu-account-details.h"
#include "gadu-contact-account-data.h"
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
	
	Account *defaultGaduGadu = new Account();
	GaduAccountDetails *accountDetails = new GaduAccountDetails(defaultGaduGadu->storage(), defaultGaduGadu);
	defaultGaduGadu->setDetails(accountDetails);

	defaultGaduGadu->setName("Gadu-Gadu");
	defaultGaduGadu->setId(config_file.readEntry("General", "UIN"));
	defaultGaduGadu->setPassword(unicode2cp(pwHash(config_file.readEntry("General", "Password"))));
	defaultGaduGadu->setRememberPassword(true);
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

	defaultGaduGadu->setUseProxy(config_file.readBoolEntry("Network", "UseProxy"));
	if (!host.setAddress(config_file.readEntry("Network", "ProxyHost")))
		host.setAddress("0.0.0.0");
	defaultGaduGadu->setProxyHost(host);
	defaultGaduGadu->setProxyPassword(config_file.readEntry("Network", "ProxyPassword"));
	defaultGaduGadu->setProxyPort(config_file.readNumEntry("Network", "ProxyPort"));
	defaultGaduGadu->setProxyUser(config_file.readEntry("Network", "ProxyUser"));
	defaultGaduGadu->setProxyRequiresAuthentication(!defaultGaduGadu->proxyUser().isEmpty());

	accountDetails->import_0_6_5_LastStatus();

	AccountManager::instance()->registerAccount(defaultGaduGadu);
}

void GaduImporter::importContacts()
{
	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact &)),
			this, SLOT(contactAdded(Contact &)));

	foreach (Contact contact, ContactManager::instance()->contacts())
		contactAdded(contact);

	importIgnored();
}

void GaduImporter::importGaduContact(Contact& contact)
{
	Account *account = AccountManager::instance()->defaultAccount();
	QString id = contact.customData()["uin"];

	GaduContactAccountData *gcad = new GaduContactAccountData(account, contact, id, true);

	gcad->setBlocked(QVariant(contact.customData()["blocking"]).toBool());
	gcad->setOfflineTo(QVariant(contact.customData()["offline_to"]).toBool());

	contact.customData().remove("uin");
	contact.customData().remove("blocking");
	contact.customData().remove("offline_to");

	contact.addAccountData(gcad);

	ContactAccountDataManager::instance()->addContactAccountData(gcad);
}

void GaduImporter::importIgnored()
{
	Account *account = AccountManager::instance()->defaultAccount();
	if (!account)
		return;

	QDomElement ignored = xml_config_file->getNode("Ignored", XmlConfigFile::ModeFind);
	if (ignored.isNull())
		return;

	QDomNodeList ignoredGroups = xml_config_file->getNodes(ignored, "IgnoredGroup");
	for (int i = 0; i < ignoredGroups.count(); i++)
	{
		QDomElement ignoredGroup = ignoredGroups.item(i).toElement();
		if (ignoredGroup.isNull())
			continue;

		ContactSet ignoredList;
		QDomNodeList ignoredContacts = xml_config_file->getNodes(ignoredGroup, "IgnoredContact");
		for (int j = 0; j < ignoredContacts.count(); j++)
		{
			QDomElement ignoredContact = ignoredContacts.item(j).toElement();
			if (ignoredContact.isNull())
				continue;

			ignoredList.insert(ContactManager::instance()->byId(account, ignoredContact.attribute("uin")));
		}

		if (0 == ignoredList.count())
			continue;

		IgnoredHelper::setIgnored(ignoredList);
	}
}

void GaduImporter::contactAdded(Contact &contact)
{
	if (contact.customData().contains("uin"))
		importGaduContact(contact);
}
