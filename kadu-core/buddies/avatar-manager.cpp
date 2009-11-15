/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "accounts/account.h"
#include "buddies/avatar.h"
#include "buddies/avatar-shared.h"
#include "contacts/contact.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"
#include "debug.h"

#include "avatar-manager.h"

AvatarManager * AvatarManager::Instance = 0;

AvatarManager * AvatarManager::instance()
{
	if (!Instance)
		Instance = new AvatarManager();

	return Instance;
}

AvatarManager::AvatarManager()
{
	triggerAllAccountsRegistered();
}

AvatarManager::~AvatarManager()
{
	triggerAllAccountsUnregistered();
}

void AvatarManager::load()
{
	if (!isValidStorage())
		return;
	
	if (isLoaded())
		return;
	
	StorableObject::load();
	
	QDomElement avatarsNode = storage()->point();
	if (avatarsNode.isNull())
		return;
	
	QList<QDomElement> avatarElements = storage()->storage()->getNodes(avatarsNode, "Avatar");
	foreach (QDomElement avatarElement, avatarElements)
	{
		StoragePoint *storagePoint = new StoragePoint(storage()->storage(), avatarElement);
		Avatar avatar = Avatar::loadFromStorage(storagePoint);
		addAvatar(avatar);
	}
}

void AvatarManager::store()
{
	if (!isValidStorage())
		return;

	StorableObject::ensureLoaded();

	foreach (Avatar avatar, Avatars)
		avatar.store();
}

void AvatarManager::addAvatar(Avatar avatar)
{
	if (avatar.isNull())
		return;

	StorableObject::ensureLoaded();

	if (Avatars.contains(avatar))
		return;

	emit avatarAboutToBeAdded(avatar);
	Avatars.append(avatar);
	emit avatarAdded(avatar);
}

void AvatarManager::removeAvatar(Avatar avatar)
{
	kdebugf();

	if (avatar.isNull())
		return;

	StorableObject::ensureLoaded();

	if (!Avatars.contains(avatar))
		return;

	emit avatarAboutToBeRemoved(avatar);
	Avatars.removeAll(avatar);
	emit avatarRemoved(avatar);
}

Avatar AvatarManager::byIndex(unsigned int index)
{
	StorableObject::ensureLoaded();

	if (index < 0 || index >= count())
		return Avatar::null;

	return Avatars.at(index);
}

Avatar AvatarManager::byUuid(const QString &uuid)
{
	StorableObject::ensureLoaded();

	if (uuid.isEmpty())
		return Avatar::null;

	foreach (Avatar avatar, Avatars)
		if (uuid == avatar.uuid().toString())
			return avatar;

	return Avatar::null;
}

AvatarService * AvatarManager::avatarService(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return 0;

	return protocol->avatarService();
}

AvatarService * AvatarManager::avatarService(Contact contact)
{
	Account account = contact.contactAccount();
	if (account.isNull())
		return 0;

	return avatarService(account);
}

QString AvatarManager::avatarFileName(Avatar avatar)
{
	Contact contact = avatar.avatarContact();
	if (contact.isNull())
		return QString::null;

	Account account = contact.contactAccount();
	if (account.isNull())
		return QString::null;

	return QString("%1-%2").arg(contact.ownerBuddy().uuid().toString(), account.uuid().toString());
}

StoragePoint * AvatarManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Avatars"));
}

void AvatarManager::accountRegistered(Account account)
{
	AvatarService *service = avatarService(account);
	if (!service)
		return;

	connect(service, SIGNAL(avatarFetched(Contact, const QByteArray &)),
			this, SLOT(avatarFetched(Contact, const QByteArray &)));
}

void AvatarManager::accountUnregistered(Account account)
{
	AvatarService *service = avatarService(account);
	if (!service)
		return;

	disconnect(service, SIGNAL(avatarFetched(Contact, const QByteArray &)),
			   this, SLOT(avatarFetched(Contact, const QByteArray &)));
}

void AvatarManager::updateAvatar(Contact contact)
{
	QDateTime lastUpdated = contact.contactAvatar().lastUpdated();
	QDateTime nextUpdate = contact.contactAvatar().nextUpdate();
	if (lastUpdated.isValid() && lastUpdated.secsTo(QDateTime::currentDateTime()) < 60*60 || QFile::exists(contact.contactAvatar().filePath()) && nextUpdate > QDateTime::currentDateTime())
		return;

	AvatarService *service = avatarService(contact);
	if (!service)
		return;

	service->fetchAvatar(contact);
}

void AvatarManager::avatarFetched(Contact contact, const QByteArray &data)
{
	Avatar avatar = contact.contactAvatar();
	avatar.setLastUpdated(QDateTime::currentDateTime());

	QPixmap pixmap;
	pixmap.loadFromData(data);
	avatar.setPixmap(pixmap);

	QString avatarFile = avatarFileName(avatar);
	avatar.setFileName(avatarFile);

	QDir avatarsDir(ggPath("avatars"));
	if (!avatarsDir.exists())
		avatarsDir.mkpath(ggPath("avatars"));

	QFile file(avatarsDir.canonicalPath() + "/" + avatarFile);
	if (!file.open(QIODevice::WriteOnly))
		return;

	file.write(data);
	file.close();

	emit avatarUpdated(contact);
}
