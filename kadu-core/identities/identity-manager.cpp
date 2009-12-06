/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "core/core.h"
#include "configuration/configuration-manager.h"
#include "identity.h"

#include "identity-manager.h"

IdentityManager * IdentityManager::Instance = 0;

KADUAPI IdentityManager * IdentityManager::instance()
{
	if (0 == Instance)
		Instance = new IdentityManager();

	return Instance;
}

IdentityManager::IdentityManager()
{
}

IdentityManager::~IdentityManager()
{
}

Identity IdentityManager::byName(const QString &name, bool create)
{
	if (name.isEmpty())
		return Identity::null;

	foreach (Identity identity, items())
	{
		if (name == identity.name())
			return identity;
	}

	if (!create)
		return Identity::null;

	Identity newIdentity = Identity::create();
	newIdentity.setName(name);
	addItem(newIdentity);

	return newIdentity;
}

Identity IdentityManager::identityForAcccount(Account account)
{
	foreach (Identity identity, items())
		if (identity.hasAccount(account))
			return identity;

	return Identity::null;
}


void IdentityManager::itemAboutToBeAdded(Identity item)
{
	emit identityAboutToBeRegistered(item);
}

void IdentityManager::itemAdded(Identity item)
{
	emit identityRegistered(item);
}

void IdentityManager::itemAboutToBeRemoved(Identity item)
{
	emit identityAboutToBeUnregistered(item);
}

void IdentityManager::itemRemoved(Identity item)
{
	emit identityUnregistered(item);
}
