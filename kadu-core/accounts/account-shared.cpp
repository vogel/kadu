/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "identities/identity-manager.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "status/status-changer-manager.h"

#include "account-shared.h"

AccountShared * AccountShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	AccountShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

AccountShared * AccountShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	AccountShared *result = new AccountShared();
	result->setStorage(storagePoint);

	return result;
}

AccountShared::AccountShared(QUuid uuid) :
		BaseStatusContainer(this), Shared(uuid),
		ProtocolHandler(0), RememberPassword(false), HasPassword(false), Removing(false)
{
}

AccountShared::~AccountShared()
{
	ref.ref();

	triggerAllProtocolsUnregistered();

	if (ProtocolHandler)
	{
		delete ProtocolHandler;
		ProtocolHandler = 0;
	}
}

StorableObject * AccountShared::storageParent()
{
	return AccountManager::instance();
}

QString AccountShared::storageNodeName()
{
	return QLatin1String("Account");
}

void AccountShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Identity identity = IdentityManager::instance()->byUuid(loadValue<QString>("Identity"));
	if (identity.isNull() && !IdentityManager::instance()->items().isEmpty())
		identity = IdentityManager::instance()->items().at(0);

	setAccountIdentity(identity);

	ProtocolName = loadValue<QString>("Protocol");
	setId(loadValue<QString>("Id"));

	RememberPassword = loadValue<bool>("RememberPassword", true);
	HasPassword = RememberPassword;
	if (RememberPassword)
		Password = pwHash(loadValue<QString>("Password"));

	ProxySettings.setEnabled(loadValue<bool>("UseProxy"));
	ProxySettings.setAddress(loadValue<QString>("ProxyHost"));
	ProxySettings.setPort(loadValue<int>("ProxyPort"));
	ProxySettings.setRequiresAuthentication(loadValue<bool>("ProxyRequiresAuthentication"));
	ProxySettings.setUser(loadValue<QString>("ProxyUser"));
	ProxySettings.setPassword(loadValue<QString>("ProxyPassword"));

	PrivateStatus = loadValue<bool>("PrivateStatus", true);

	triggerAllProtocolsRegistered();
}

void AccountShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Identity", AccountIdentity.uuid().toString());

	storeValue("Protocol", ProtocolName);
	storeValue("Id", id());

	storeValue("RememberPassword", RememberPassword);
	if (RememberPassword && HasPassword)
		storeValue("Password", pwHash(password()));
	else
		removeValue("Password");

	storeValue("UseProxy", ProxySettings.enabled());
	storeValue("ProxyHost", ProxySettings.address());
	storeValue("ProxyPort", ProxySettings.port());
	storeValue("ProxyRequiresAuthentication", ProxySettings.requiresAuthentication());
	storeValue("ProxyUser", ProxySettings.user());
	storeValue("ProxyPassword", ProxySettings.password());

	storeValue("PrivateStatus", PrivateStatus);
}

bool AccountShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore() &&
			!Id.isEmpty();
}

void AccountShared::aboutToBeRemoved()
{
	setDetails(0);
}

void AccountShared::emitUpdated()
{
	emit updated();
}

void AccountShared::setDisconnectStatus()
{
	if (status().type() == "Offline")
		return;

	bool disconnectWithCurrentDescription = config_file.readBoolEntry("General", "DisconnectWithCurrentDescription");
	QString disconnectDescription = config_file.readEntry("General", "DisconnectDescription");

	Status disconnectStatus;
	disconnectStatus.setType("Offline");

	if (disconnectWithCurrentDescription)
		disconnectStatus.setDescription(status().description());
	else
		disconnectStatus.setDescription(disconnectDescription);

	doSetStatus(disconnectStatus); // this does not store status
}

void AccountShared::useProtocolFactory(ProtocolFactory *factory)
{
	Protocol *oldProtocolHandler = ProtocolHandler;

	if (ProtocolHandler)
	{
		disconnect(ProtocolHandler, SIGNAL(statusChanged(Account, Status)), this, SIGNAL(statusChanged()));
		disconnect(ProtocolHandler, SIGNAL(contactStatusChanged(Contact, Status)),
				   this, SIGNAL(buddyStatusChanged(Contact, Status)));
		disconnect(ProtocolHandler, SIGNAL(connected(Account)), this, SIGNAL(connected()));
		disconnect(ProtocolHandler, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));

		storeStatus(StatusChangerManager::instance()->manuallySetStatus(this));
		setDisconnectStatus();
	}

	if (!factory)
	{
		setDetails(0);
		ProtocolHandler = 0;
		emit protocolUnloaded();
	}
	else
	{
		ProtocolHandler = factory->createProtocolHandler(this);
		setDetails(factory->createAccountDetails(this));
		emit protocolLoaded();
	}

	if (oldProtocolHandler)
		delete oldProtocolHandler;

	if (ProtocolHandler)
	{
		connect(ProtocolHandler, SIGNAL(statusChanged(Account, Status)), this, SIGNAL(statusChanged()));
		connect(ProtocolHandler, SIGNAL(contactStatusChanged(Contact, Status)),
				this, SIGNAL(buddyStatusChanged(Contact, Status)));
		connect(ProtocolHandler, SIGNAL(connected(Account)), this, SIGNAL(connected()));
		connect(ProtocolHandler, SIGNAL(disconnected(Account)), this, SIGNAL(disconnected()));
	}
}

void AccountShared::protocolRegistered(ProtocolFactory *factory)
{
	ensureLoaded();

	if (factory->name() != ProtocolName)
		return;

	if (ProtocolHandler && (ProtocolHandler->protocolFactory() == factory))
		return;

	useProtocolFactory(factory);
}

void AccountShared::protocolUnregistered(ProtocolFactory* factory)
{
	ensureLoaded();

	if (!ProtocolHandler)
		return;

	if (factory != ProtocolHandler->protocolFactory())
		return;

	useProtocolFactory(0);
}

void AccountShared::detailsAdded()
{
	details()->ensureLoaded();

	AccountManager::instance()->detailsLoaded(this);
}

void AccountShared::detailsAboutToBeRemoved()
{
	details()->store();
}

void AccountShared::detailsRemoved()
{
	AccountManager::instance()->detailsUnloaded(this);
}

void AccountShared::setAccountIdentity(Identity accountIdentity)
{
	ensureLoaded();

	if (AccountIdentity == accountIdentity)
		return;

	/* NOTE: This guard is needed to avoid deleting this object when removing
	 * Account from Identity which may hold last reference to it and thus wants
	 * to delete it.
	 */
	Account guard(this);

	AccountIdentity.removeAccount(this);
	AccountIdentity = accountIdentity;
	AccountIdentity.addAccount(this);

	dataUpdated();
}

void AccountShared::setProtocolName(QString protocolName)
{
	ensureLoaded();

	ProtocolName = protocolName;
	useProtocolFactory(ProtocolsManager::instance()->byName(protocolName));

	dataUpdated();
}

void AccountShared::setId(const QString &id)
{
	ensureLoaded();

	if (Id == id)
		return;

	Id = id;
	AccountContact.setId(id);

	dataUpdated();
}

Contact AccountShared::accountContact()
{
	ensureLoaded();

	if (AccountContact.isNull())
	{
		AccountContact = ContactManager::instance()->byId(this, Id, ActionCreateAndAdd);
		ContactManager::instance()->addItem(AccountContact);
	}

	return AccountContact;
}

QString AccountShared::statusContainerName()
{
	return Id;
}

void AccountShared::doSetStatus(Status status)
{
	if (ProtocolHandler)
		ProtocolHandler->setStatus(status);
}

Status AccountShared::status()
{
	if (ProtocolHandler)
		return ProtocolHandler->status();
	else
		return Status();
}

int AccountShared::maxDescriptionLength()
{
	if (ProtocolHandler)
		return ProtocolHandler->maxDescriptionLength();
	else
		return 0;
}

QString AccountShared::statusDisplayName()
{
	return status().displayName();
}

QIcon AccountShared::statusIcon()
{
	return statusIcon(status());
}

QString AccountShared::statusIconPath(const QString &statusType)
{
	if (ProtocolHandler)
		return ProtocolHandler->statusIconFullPath(statusType);
	else
		return QString();
}

QIcon AccountShared::statusIcon(const QString &statusType)
{
	if (ProtocolHandler)
		return ProtocolHandler->statusIcon(statusType);
	else
		return QIcon();
}

QIcon AccountShared::statusIcon(Status status)
{
	if (ProtocolHandler)
		return ProtocolHandler->statusIcon(status);
	else
		return QIcon();
}

void AccountShared::setPrivateStatus(bool isPrivate)
{
	if (PrivateStatus == isPrivate)
		return;

	PrivateStatus = isPrivate;

	if (ProtocolHandler)
		ProtocolHandler->changePrivateMode();
}

QList<StatusType *> AccountShared::supportedStatusTypes()
{
	if (ProtocolHandler)
		return ProtocolHandler->protocolFactory()->supportedStatusTypes();
	else
		return QList<StatusType *>();
}

void AccountShared::fileTransferServiceChanged(FileTransferService *service)
{
	if (service)
		emit fileTransferServiceRegistered();
	else
		emit fileTransferServiceUnregistered();
}
