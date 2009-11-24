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

IdentityManager::IdentityManager() :
		StorableObject(StorableObject::StateLoaded)
{
    	ConfigurationManager::instance()->registerStorableObject(this);
}

IdentityManager::~IdentityManager()
{
}

StoragePoint * IdentityManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Identities"));
}

void IdentityManager::load()
{
	StorableObject::load();

	QDomElement identitiesNode = xml_config_file->getNode("Identities", XmlConfigFile::ModeFind);
	if (identitiesNode.isNull())
		return;

	QDomNodeList identityNodes = identitiesNode.elementsByTagName("Identity");

	int count = identityNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement identityElement = identityNodes.at(i).toElement();
		if (identityElement.isNull())
			continue;

		StoragePoint *identityStoragePoint = new StoragePoint(xml_config_file, identityElement);
		registerIdentity(new Identity(IdentityShared::loadFromStorage(identityStoragePoint)));
	}
}

void IdentityManager::store()
{
	if (!isValidStorage())
		return;

	QDomElement identityNode = storage()->point();

	foreach (Identity *identity, Identities)
		identity->data()->store();
}

Identity * IdentityManager::byUuid(const QString &uuid)
{
    	if (uuid.isEmpty())
		return 0;

	foreach (Identity *identity, Identities)
		if (uuid == identity->uuid())
			return identity;

	return 0;
}

Identity * IdentityManager::byName(const QString &name, bool create)
{
	if (name.isEmpty())
		return 0;

	foreach (Identity *identity, Identities)
	{
		if (name == identity->name())
			return identity;
	}

	if (!create)
		return 0;

	Identity *newIdentity = new Identity(new IdentityShared(QUuid::createUuid()));
	registerIdentity(newIdentity);

	return newIdentity;
}

Identity * IdentityManager::identityForAcccount(Account account)
{
	foreach (Identity *identity, Identities)
	{
		if (identity->hasAccount(account))
			return identity;
	}
	return 0;
}

void IdentityManager::registerIdentity(Identity *identity)
{
	emit identityAboutToBeRegistered(identity);
	Identities << identity;
	emit identityRegistered(identity);
}

void IdentityManager::unregisterIdentity(Identity *identity)
{
	emit identityAboutToBeUnregistered(identity);
	Identities.removeAll(identity);
	emit identityUnregistered(identity);
}

void IdentityManager::deleteIdentity(Identity *identity)
{
	unregisterIdentity(identity);

	identity->data()->removeFromStorage();
}
