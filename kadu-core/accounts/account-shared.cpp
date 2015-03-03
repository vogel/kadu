/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "core/application.h"
#include "icons/kadu-icon.h"
#include "identities/identity-manager.h"
#include "identities/identity.h"
#include "misc/change-notifier.h"
#include "misc/misc.h"
#include "network/proxy/network-proxy-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "roster/roster-service-tasks.h"
#include "roster/roster-service.h"
#include "roster/roster-task-collection-storage.h"
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

QVector<RosterTask> AccountShared::loadRosterTasks()
{
	if (!isValidStorage())
		return {};

	auto tasksStorage = RosterTaskCollectionStorage{storage()};
	return tasksStorage.loadRosterTasks();
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

	if (protocolHandler() && protocolHandler()->rosterService() && protocolHandler()->rosterService()->tasks())
		protocolHandler()->rosterService()->tasks()->addTasks(loadRosterTasks());
}

void AccountShared::storeRosterTasks(const QVector<RosterTask> &tasks)
{
	if (!isValidStorage())
		return;

	auto tasksStorage = RosterTaskCollectionStorage{storage()};
	tasksStorage.storeRosterTasks(tasks);
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

	if (protocolHandler() && protocolHandler()->rosterService() && protocolHandler()->rosterService()->tasks())
		storeRosterTasks(protocolHandler()->rosterService()->tasks()->tasks());
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

	bool disconnectWithCurrentDescription = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "DisconnectWithCurrentDescription");
	QString disconnectDescription = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "DisconnectDescription");

	Status disconnectStatus;
	disconnectStatus.setType(StatusTypeOffline);

	if (disconnectWithCurrentDescription)
		disconnectStatus.setDescription(MyStatusContainer->status().description());
	else
		disconnectStatus.setDescription(disconnectDescription);

	StatusSetter::instance()->setStatusManually(MyStatusContainer, disconnectStatus);
}

void AccountShared::protocolRegistered(ProtocolFactory *factory)
{
	if (!factory)
		return;

	ensureLoaded();

	if (ProtocolHandler || (factory->name() != ProtocolName) || Details)
		return;

	Details = factory->createAccountDetails(this);
	if (Details)
		details()->ensureLoaded();

	ProtocolHandler = factory->createProtocolHandler(this);
	if (!ProtocolHandler)
		return;

	connect(ProtocolHandler, SIGNAL(statusChanged(Account, Status)), MyStatusContainer, SLOT(triggerStatusUpdated()));
	connect(ProtocolHandler, SIGNAL(contactStatusChanged(Contact, Status)),
			this, SIGNAL(buddyStatusChanged(Contact, Status)));
	connect(ProtocolHandler, SIGNAL(connected(Account)), this, SIGNAL(connected()));
	connect(ProtocolHandler, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));

	if (protocolHandler() && protocolHandler()->rosterService() && protocolHandler()->rosterService()->tasks())
		protocolHandler()->rosterService()->tasks()->addTasks(loadRosterTasks());

	MyStatusContainer->triggerStatusUpdated();

	AccountManager::instance()->registerItem(this);

	emit updated();
	emit protocolHandlerChanged();
}

void AccountShared::protocolUnregistered(ProtocolFactory* factory)
{
	if (!factory)
		return;

	ensureLoaded();

	if (!ProtocolHandler || (factory->name() != ProtocolName) || !Details)
		return;

	if (protocolHandler() && protocolHandler()->rosterService() && protocolHandler()->rosterService()->tasks())
		storeRosterTasks(protocolHandler()->rosterService()->tasks()->tasks());

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
	emit protocolHandlerChanged();
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

KaduShared_PropertyPtrReadDef(AccountShared, Identity, accountIdentity, AccountIdentity)

#include "moc_account-shared.cpp"
