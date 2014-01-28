/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "accounts/account-status-container.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity-manager.h"
#include "identities/identity.h"
#include "misc/change-notifier.h"
#include "misc/misc.h"
#include "network/proxy/network-proxy-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "protocols/services/roster/roster-service.h"
#include "status/status-setter.h"

#include "account-shared.h"

AccountShared * AccountShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	AccountShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

AccountShared * AccountShared::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	AccountShared *result = new AccountShared();
	result->setStorage(storagePoint);

	return result;
}

AccountShared::AccountShared(const QString &protocolName) :
		QObject(), Shared(QUuid()), ProtocolName(protocolName),
		ProtocolHandler(0), MyStatusContainer(new AccountStatusContainer(this)), Details(0),
		RememberPassword(false), HasPassword(false), UseDefaultProxy(true), PrivateStatus(true)
{
	AccountIdentity = new Identity();
	AccountContact = new Contact();

	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory*)),
	        this, SLOT(protocolRegistered(ProtocolFactory*)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory*)),
	        this, SLOT(protocolUnregistered(ProtocolFactory*)));

	// ProtocolName is not empty here only if a new Account has just been created
	// it that case load() method will not be called so we need to call triggerAllProtocolsRegistered here
	if (!ProtocolName.isEmpty())
	{
		ProtocolFactory *factory = ProtocolsManager::instance()->byName(ProtocolName);
		if (factory)
			protocolRegistered(factory);
	}

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

AccountShared::~AccountShared()
{
	ref.ref();

	if (!ProtocolName.isEmpty())
	{
		ProtocolFactory *factory = ProtocolsManager::instance()->byName(ProtocolName);
		if (factory)
			protocolUnregistered(factory);
	}

	delete MyStatusContainer;
	MyStatusContainer = 0;

	delete ProtocolHandler;
	ProtocolHandler = 0;

	delete AccountContact;
	delete AccountIdentity;
}

StorableObject * AccountShared::storageParent()
{
	return AccountManager::instance();
}

QString AccountShared::storageNodeName()
{
	return QLatin1String("Account");
}

void AccountShared::importNetworkProxy()
{
	QString address = loadValue<QString>("ProxyHost");

	int port = loadValue<int>("ProxyPort");
	bool requiresAuthentication = loadValue<bool>("ProxyRequiresAuthentication");
	QString user = loadValue<QString>("ProxyUser");
	QString password = loadValue<QString>("ProxyPassword");

	if (!requiresAuthentication)
	{
		user.clear();
		password.clear();
	}

	NetworkProxy importedProxy;

	if (!address.isEmpty())
		importedProxy = NetworkProxyManager::instance()->byConfiguration(
		            address, port, user, password, ActionCreateAndAdd);

	if (loadValue<bool>("UseProxy"))
		Proxy = importedProxy;

	removeValue("UseProxy");
	removeValue("ProxyHost");
	removeValue("ProxyPort");
	removeValue("ProxyRequiresAuthentication");
	removeValue("ProxyUser");
	removeValue("ProxyPassword");
}

void AccountShared::loadRosterTasks()
{
	if (!isValidStorage())
		return;

	if (!protocolHandler() || !protocolHandler()->rosterService())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement rosterTasksNode = configurationStorage->getNode(storage()->point(), "RosterTasks");

	QDomNodeList rosterTaskNodes = rosterTasksNode.childNodes();
	const int rosterTaskCount = rosterTaskNodes.count();

	for (int i = 0; i < rosterTaskCount; i++)
	{
		QDomElement rosterTaskElement = rosterTaskNodes.at(i).toElement();
		if (rosterTaskElement.isNull() || rosterTaskElement.text().isEmpty())
			continue;

		if (rosterTaskElement.nodeName() == "Add")
			protocolHandler()->rosterService()->addTask(RosterTask(RosterTaskAdd, rosterTaskElement.text()));
		else if (rosterTaskElement.nodeName() == "Delete")
			protocolHandler()->rosterService()->addTask(RosterTask(RosterTaskDelete, rosterTaskElement.text()));
		else if (rosterTaskElement.nodeName() == "Update")
			protocolHandler()->rosterService()->addTask(RosterTask(RosterTaskUpdate, rosterTaskElement.text()));
	}
}

void AccountShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Identity identity = IdentityManager::instance()->byUuid(loadValue<QString>("Identity"));
	if (identity.isNull() && !IdentityManager::instance()->items().isEmpty())
		identity = IdentityManager::instance()->items().at(0);
	doSetAccountIdentity(identity);

	ProtocolName = loadValue<QString>("Protocol");

	doSetId(loadValue<QString>("Id"));

	RememberPassword = loadValue<bool>("RememberPassword", true);
	HasPassword = RememberPassword;
	if (RememberPassword)
		Password = pwHash(loadValue<QString>("Password"));

	if (hasValue("UseProxy"))
	{
		UseDefaultProxy = false;
		importNetworkProxy();
	}
	else
	{
		UseDefaultProxy = loadValue<bool>("UseDefaultProxy", true);
		if (!UseDefaultProxy)
			Proxy = NetworkProxyManager::instance()->byUuid(loadValue<QString>("Proxy"));
	}

	PrivateStatus = loadValue<bool>("PrivateStatus", true);

	if (!ProtocolName.isEmpty())
	{
		ProtocolFactory *factory = ProtocolsManager::instance()->byName(ProtocolName);
		if (factory)
			protocolRegistered(factory);
	}

	loadRosterTasks();
}

void AccountShared::storeRosterTasks()
{
	if (!isValidStorage())
		return;

	if (!protocolHandler() || !protocolHandler()->rosterService())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement rosterTasksNode = configurationStorage->getNode(storage()->point(), "RosterTasks");

	while (!rosterTasksNode.childNodes().isEmpty())
		rosterTasksNode.removeChild(rosterTasksNode.childNodes().at(0));

	QVector<RosterTask> tasks = protocolHandler()->rosterService()->tasks();
	foreach (const RosterTask &task, tasks)
		switch (task.type())
		{
			case RosterTaskAdd:
				configurationStorage->createTextNode(rosterTasksNode, "Add", task.id());
				break;
			case RosterTaskDelete:
				configurationStorage->createTextNode(rosterTasksNode, "Delete", task.id());
				break;
			case RosterTaskUpdate:
				configurationStorage->createTextNode(rosterTasksNode, "Update", task.id());
				break;
			default:
				break;
		}
}

void AccountShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Identity", AccountIdentity->uuid().toString());
	storeValue("UseDefaultProxy", UseDefaultProxy);
	if (UseDefaultProxy)
		removeValue("Proxy");
	else
		storeValue("Proxy", Proxy.uuid().toString());

	storeValue("Protocol", ProtocolName);
	storeValue("Id", id());

	storeValue("RememberPassword", RememberPassword);
	if (RememberPassword && HasPassword)
		storeValue("Password", pwHash(password()));
	else
		removeValue("Password");

	storeValue("PrivateStatus", PrivateStatus);

	if (Details)
		Details->ensureStored();

	storeRosterTasks();
}

bool AccountShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore() &&
			!Id.isEmpty();
}

void AccountShared::aboutToBeRemoved()
{
	if (Details)
	{
		Details->ensureStored();
		delete Details;
		Details = 0;
	}

	AccountManager::instance()->unregisterItem(this);
	setAccountIdentity(Identity::null);
}

void AccountShared::forceEmitUpdated()
{
	emit updated();
}

void AccountShared::setDisconnectStatus()
{
	if (!ProtocolHandler)
		return;
	if (!ProtocolHandler->isConnected() && !ProtocolHandler->isDisconnecting())
		return;

	bool disconnectWithCurrentDescription = config_file.readBoolEntry("General", "DisconnectWithCurrentDescription");
	QString disconnectDescription = config_file.readEntry("General", "DisconnectDescription");

	Status disconnectStatus;
	disconnectStatus.setType(StatusTypeOffline);

	if (disconnectWithCurrentDescription)
		disconnectStatus.setDescription(MyStatusContainer->status().description());
	else
		disconnectStatus.setDescription(disconnectDescription);

	StatusSetter::instance()->setStatus(MyStatusContainer, disconnectStatus);
}

void AccountShared::protocolRegistered(ProtocolFactory *factory)
{
	if (!factory)
		return;

	ensureLoaded();

	if (ProtocolHandler || (factory->name() != ProtocolName) || Details)
		return;

	ProtocolHandler = factory->createProtocolHandler(this);
	if (!ProtocolHandler)
		return;

	Details = factory->createAccountDetails(this);
	if (Details)
		details()->ensureLoaded();

	connect(ProtocolHandler, SIGNAL(statusChanged(Account, Status)), MyStatusContainer, SLOT(triggerStatusUpdated()));
	connect(ProtocolHandler, SIGNAL(contactStatusChanged(Contact, Status)),
			this, SIGNAL(buddyStatusChanged(Contact, Status)));
	connect(ProtocolHandler, SIGNAL(connected(Account)), this, SIGNAL(connected()));
	connect(ProtocolHandler, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));

	loadRosterTasks();

	MyStatusContainer->triggerStatusUpdated();

	AccountManager::instance()->registerItem(this);

	emit updated();
}

void AccountShared::protocolUnregistered(ProtocolFactory* factory)
{
	if (!factory)
		return;

	ensureLoaded();

	if (!ProtocolHandler || (factory->name() != ProtocolName) || !Details)
		return;

	storeRosterTasks();

	disconnect(ProtocolHandler, SIGNAL(statusChanged(Account, Status)), MyStatusContainer, SLOT(triggerStatusUpdated()));
	disconnect(ProtocolHandler, 0, this, 0);

	setDisconnectStatus();

	Details->ensureStored();
	delete Details;
	Details = 0;

	// dont get deleted in next line
	Account guard(this);
	AccountManager::instance()->unregisterItem(this);

	delete ProtocolHandler;
	ProtocolHandler = 0;

	emit updated();
}

void AccountShared::doSetAccountIdentity(const Identity &accountIdentity)
{
	/* NOTE: This guard is needed to avoid deleting this object when removing
	 * Account from Identity which may hold last reference to it and thus wants
	 * to delete it.
	 */
	Account guard(this);

	AccountIdentity->removeAccount(this);
	*AccountIdentity = accountIdentity;
	AccountIdentity->addAccount(this);
}

void AccountShared::setAccountIdentity(const Identity &accountIdentity)
{
	ensureLoaded();

	if (*AccountIdentity == accountIdentity)
		return;

	doSetAccountIdentity(accountIdentity);

	changeNotifier().notify();
}

void AccountShared::doSetId(const QString &id)
{
	Id = id;
	AccountContact->setId(id);
}

void AccountShared::setId(const QString &id)
{
	ensureLoaded();

	if (Id == id)
		return;

	doSetId(id);

	changeNotifier().notify();
}

Contact AccountShared::accountContact()
{
	ensureLoaded();

	if (!*AccountContact)
		*AccountContact = ContactManager::instance()->byId(this, Id, ActionCreateAndAdd);

	return *AccountContact;
}

StatusContainer * AccountShared::statusContainer()
{
	return MyStatusContainer;
}

void AccountShared::setPrivateStatus(bool isPrivate)
{
	if (PrivateStatus == isPrivate)
		return;

	PrivateStatus = isPrivate;

	if (ProtocolHandler)
		ProtocolHandler->changePrivateMode();
}

void AccountShared::fileTransferServiceChanged(FileTransferService *service)
{
	if (service)
		emit fileTransferServiceRegistered();
	else
		emit fileTransferServiceUnregistered();
}

KaduShared_PropertyPtrReadDef(AccountShared, Identity, accountIdentity, AccountIdentity)

#include "moc_account-shared.cpp"
