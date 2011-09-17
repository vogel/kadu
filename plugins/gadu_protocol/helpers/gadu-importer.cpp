/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "accounts/account-shared.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "buddies/buddy.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"

#include "helpers/gadu-imported-contact-xml-receiver.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"
#include "gadu-protocol-factory.h"

#include "gadu-importer.h"

GaduImporter * GaduImporter::Instance;

const QString GaduImporter::EntryQuery("/Kadu/Deprecated/ConfigFile[ends-with(@name,'kadu.conf')]/Group[@name='%1']/Entry[@name='%2']/@value/string()");
const QString GaduImporter::ContactsQuery("/Kadu/Contacts/Contact");

void GaduImporter::createInstance()
{
	if (!Instance)
		Instance = new GaduImporter();
}

void GaduImporter::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

bool GaduImporter::alreadyImported()
{
	QDomElement node = xml_config_file->getNode("Accounts", XmlConfigFile::ModeFind);
	if (node.isNull())
		return false;

	return node.hasAttribute("import_done");
}

void GaduImporter::markImported()
{
	QDomElement node = xml_config_file->getNode("Accounts", XmlConfigFile::ModeFind);
	node.setAttribute("import_done", "true");
}

QVariant GaduImporter::readEntry(QXmlQuery &xmlQuery, const QString &groupName, const QString &entryName, const QVariant &defaultValue)
{
	xmlQuery.setQuery(EntryQuery.arg(groupName).arg(entryName));

	QString result;
	if (xmlQuery.evaluateTo(&result))
		return result.trimmed();
	else
		return defaultValue;
}

Account GaduImporter::import065Account(QXmlQuery &xmlQuery)
{
	Account result = Account::create();
	result.setProtocolName("gadu");

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(result.details());
	accountDetails->setState(StorableObject::StateNew);

	result.setId(readEntry(xmlQuery, "General", "UIN").toString());
	result.setPassword(pwHash(readEntry(xmlQuery, "General", "Password").toString()));
	result.setRememberPassword(true);
	result.setHasPassword(!result.password().isEmpty());
	result.setPrivateStatus(readEntry(xmlQuery, "General", "PrivateStatus").toBool());
	accountDetails->setAllowDcc(readEntry(xmlQuery, "Network", "AllowDCC").toBool());

	accountDetails->setMaximumImageSize(readEntry(xmlQuery, "Chat", "MaxImageSize", 255).toUInt());
	accountDetails->setReceiveImagesDuringInvisibility(readEntry(xmlQuery, "Chat", "ReceiveImagesDuringInvisibility").toBool());
	accountDetails->setMaximumImageRequests(readEntry(xmlQuery, "Chat", "MaxImageRequests").toUInt());

	AccountProxySettings proxySettings;
	proxySettings.setEnabled(readEntry(xmlQuery, "Network", "UseProxy").toBool());
	proxySettings.setAddress(readEntry(xmlQuery, "Network", "ProxyHost").toString());
	proxySettings.setPort(readEntry(xmlQuery, "Network", "ProxyPort").toUInt());
	proxySettings.setUser(readEntry(xmlQuery, "Network", "ProxyUser").toString());
	proxySettings.setPassword(readEntry(xmlQuery, "Network", "ProxyPassword").toString());
	proxySettings.setRequiresAuthentication(!proxySettings.user().isEmpty());

	result.setProxySettings(proxySettings);

	return result;
}

QList<Buddy> GaduImporter::import065Buddies(Account account, QXmlQuery &xmlQuery)
{
	QList<Buddy> result;

	GaduImportedContactXmlReceiver Receiver(xmlQuery.namePool());

	xmlQuery.setQuery(ContactsQuery);
	xmlQuery.evaluateTo(&Receiver);

	result = Receiver.importedBuddies();

	foreach (Buddy buddy, result)
	{
		buddy.importConfiguration();

		if (!buddy.customData("uin").isEmpty())
			importGaduContact(account, buddy);
	}

	return result;
}

void GaduImporter::importAccounts()
{
	if (0 == config_file.readNumEntry("General", "UIN"))
		return;

	if (alreadyImported())
		return;

	if (AccountManager::instance()->byId("gadu", config_file.readEntry("General", "UIN")))
		return;

	Account defaultGaduGadu = Account::create();
	defaultGaduGadu.setProtocolName("gadu");

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(defaultGaduGadu.details());
	accountDetails->setState(StorableObject::StateNew);

	if (!IdentityManager::instance()->items().isEmpty())
		defaultGaduGadu.setAccountIdentity(IdentityManager::instance()->items().at(0));

	defaultGaduGadu.setId(config_file.readEntry("General", "UIN"));
	defaultGaduGadu.setPassword(pwHash(config_file.readEntry("General", "Password")).toUtf8().constData());
	defaultGaduGadu.setRememberPassword(true);
	defaultGaduGadu.setHasPassword(!defaultGaduGadu.password().isEmpty());
	defaultGaduGadu.setPrivateStatus(config_file.readBoolEntry("General", "PrivateStatus"));
	accountDetails->setAllowDcc(config_file.readBoolEntry("Network", "AllowDCC"));

	accountDetails->setMaximumImageSize(config_file.readNumEntry("Chat", "MaxImageSize", 255));
	accountDetails->setReceiveImagesDuringInvisibility(config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility"));
	accountDetails->setMaximumImageRequests(config_file.readNumEntry("Chat", "MaxImageRequests"));

	AccountProxySettings proxySettings;
	proxySettings.setEnabled(config_file.readBoolEntry("Network", "UseProxy"));
	proxySettings.setAddress(config_file.readEntry("Network", "ProxyHost"));
	proxySettings.setPort(config_file.readNumEntry("Network", "ProxyPort"));
	proxySettings.setUser(config_file.readEntry("Network", "ProxyUser"));
	proxySettings.setPassword(config_file.readEntry("Network", "ProxyPassword"));
	proxySettings.setRequiresAuthentication(!proxySettings.user().isEmpty());

	defaultGaduGadu.setProxySettings(proxySettings);

	accountDetails->import_0_6_5_LastStatus();

	AccountManager::instance()->addItem(defaultGaduGadu);
	defaultGaduGadu.accountContact().setOwnerBuddy(Core::instance()->myself());

	config_file.writeEntry("General", "SimpleMode", true);

	markImported();
}

void GaduImporter::importContacts()
{
	connect(BuddyManager::instance(), SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));

	foreach (Buddy buddy, BuddyManager::instance()->items())
		buddyAdded(buddy);

	importIgnored();
}

Contact GaduImporter::importGaduContact(Account account, Buddy buddy)
{
	QString id = buddy.customData("uin");

	Contact contact = ContactManager::instance()->byId(account, id, ActionCreateAndAdd);

	buddy.removeCustomData("uin");
	buddy.setBlocked(QVariant(buddy.customData("blocking")).toBool());
	buddy.setOfflineTo(QVariant(buddy.customData("offline_to")).toBool());
	buddy.removeCustomData("blocking");
	buddy.removeCustomData("offline_to");

	contact.setOwnerBuddy(buddy);

	return contact;
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
	foreach (const QDomElement &ignoredGroup, ignoredGroups)
	{
		QList<QDomElement> ignoredContacts = xml_config_file->getNodes(ignoredGroup, "IgnoredContact");
		if (1 == ignoredContacts.count())
		{
			QDomElement ignoredContact = ignoredContacts.at(0);
			Buddy buddy = BuddyManager::instance()->byId(account, ignoredContact.attribute("uin"), ActionCreateAndAdd);
			buddy.setBlocked(true);
		}
	}

	xml_config_file->removeNode(xml_config_file->rootElement(), "Ignored");
}

void GaduImporter::buddyAdded(Buddy &buddy)
{
	if (buddy.customData("uin").isEmpty())
		return;

	QList<Account> allGaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	if (0 == allGaduAccounts.count())
		return;

	// take 1st one
	Account account = allGaduAccounts.at(0);

	Contact contact = importGaduContact(account, buddy);

	if (contact)
		ContactManager::instance()->addItem(contact);
}
