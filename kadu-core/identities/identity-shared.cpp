/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Piotr "ultr" Dąbrowski (ultr@ultr.pl)
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

#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "identities/identity-manager.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "status/status-type-manager.h"
#include "status/status-type.h"

#include "identity-shared.h"

IdentityShared * IdentityShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	IdentityShared *identityShared = loadFromStorage(storagePoint);
	identityShared->loadStub();

	return identityShared;
}

IdentityShared * IdentityShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	IdentityShared *identityShared = new IdentityShared();
	identityShared->setStorage(storagePoint);

	return identityShared;
}

IdentityShared::IdentityShared(const QUuid &uuid) :
		StorableStatusContainer(this), Shared(uuid), Permanent(false)
{
	setState(StateNotLoaded);
}

IdentityShared::~IdentityShared()
{
	ref.ref();
}

StorableObject * IdentityShared::storageParent()
{
	return IdentityManager::instance();
}

QString IdentityShared::storageNodeName()
{
	return QLatin1String("Identity");
}

void IdentityShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Permanent = loadValue<bool>("Permanent");
	Name = loadValue<QString>("Name");
}

void IdentityShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Permanent", Permanent);
	storeValue("Name", Name);
}

bool IdentityShared::shouldStore()
{
	ensureLoaded();

	return (UuidStorableObject::shouldStore()
			&& !Name.isEmpty())
			|| customProperties()->shouldStore();
}

void IdentityShared::aboutToBeRemoved()
{
	Accounts.clear();
}

void IdentityShared::addAccount(const Account &account)
{
	if (!account)
		return;

	ensureLoaded();

	Accounts.append(account);
	connect(account.statusContainer(), SIGNAL(statusUpdated()), this, SIGNAL(statusUpdated()));

	emit statusUpdated();
}

void IdentityShared::removeAccount(const Account &account)
{
	if (!account)
		return;

	ensureLoaded();

	if (Accounts.removeAll(account) > 0)
	{
		disconnect(account.statusContainer(), 0, this, 0);
		emit statusUpdated();
	}
}

bool IdentityShared::hasAccount(const Account &account)
{
	ensureLoaded();

	return account && Accounts.contains(account);
}

bool IdentityShared::hasAnyAccountWithDetails()
{
	ensureLoaded();

	foreach (const Account &account, Accounts)
		if (account.details())
			return true;

	return false;
}

bool IdentityShared::isEmpty()
{
	ensureLoaded();

	return Accounts.isEmpty();
}

void IdentityShared::setStatus(Status status, StatusChangeSource source)
{
	ensureLoaded();

	foreach (const Account &account, Accounts)
		if (account)
			account.statusContainer()->setStatus(status, source);
}

Status IdentityShared::status()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->status() : Status();
}

bool IdentityShared::isStatusSettingInProgress()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->isStatusSettingInProgress() : false;
}

KaduIcon IdentityShared::statusIcon()
{
	return statusIcon(status());
}

KaduIcon IdentityShared::statusIcon(const Status &status)
{
	QSet<QString> protocols;
	foreach (const Account &account, Accounts)
		protocols.insert(account.protocolName());

	if (protocols.count() > 1)
		return StatusTypeManager::instance()->statusIcon("common", status);

	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->statusIcon(status) : KaduIcon();
}

QList<StatusType> IdentityShared::supportedStatusTypes()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->supportedStatusTypes() : QList<StatusType>();
}

int IdentityShared::maxDescriptionLength()
{
	Account account = AccountManager::bestAccount(Accounts);
	return account ? account.statusContainer()->maxDescriptionLength() : -1;
}
