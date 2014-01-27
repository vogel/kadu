/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "misc/change-notifier.h"

#include "group-shared.h"

GroupShared * GroupShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	GroupShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

GroupShared * GroupShared::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	GroupShared *result = new GroupShared();
	result->setStorage(storagePoint);

	return result;
}

GroupShared::GroupShared(const QUuid &uuid) :
		Shared(uuid),
		NotifyAboutStatusChanges(false), ShowInAllGroup(false),
		OfflineToGroup(false), ShowIcon(false), ShowName(false),
		TabPosition(-1)
{
	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

GroupShared::~GroupShared()
{
	ref.ref();
}

StorableObject * GroupShared::storageParent()
{
	return GroupManager::instance();
}

QString GroupShared::storageNodeName()
{
	return QLatin1String("Group");
}

void GroupShared::importConfiguration(const QString &name)
{
	Name = name;
	Icon = config_file.readEntry("GroupIcon", name);
	NotifyAboutStatusChanges = true;
	ShowInAllGroup= true;
	OfflineToGroup= false;
	ShowIcon = !Icon.isEmpty();
	ShowName = true;
	TabPosition = -1;
}

void GroupShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Name = loadValue<QString>("Name");
	Icon = loadValue<QString>("Icon");
	NotifyAboutStatusChanges = loadValue<bool>("NotifyAboutStatusChanges", true);
	ShowInAllGroup= loadValue<bool>("ShowInAllGroup", true);
	OfflineToGroup= loadValue<bool>("OfflineTo", true);
	ShowIcon = loadValue<bool>("ShowIcon", true);
	ShowName = loadValue<bool>("ShowName", true);
	TabPosition = loadValue<int>("TabPosition", -1);
}

void GroupShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("Name", Name);
	storeValue("Icon", Icon);
	storeValue("NotifyAboutStatusChanges", NotifyAboutStatusChanges);
	storeValue("ShowInAllGroup", ShowInAllGroup);
	storeValue("OfflineTo", OfflineToGroup);
	storeValue("ShowIcon", ShowIcon);
	storeValue("ShowName", ShowName);
	storeValue("TabPosition", TabPosition);
}

void GroupShared::aboutToBeRemoved()
{
	emit groupAboutToBeRemoved();
}

void GroupShared::setName(const QString &name)
{
	ensureLoaded();

	if (Name != name)
	{
		Name = name;
		changeNotifier().notify();
		emit nameChanged();
	}
}

#include "moc_group-shared.cpp"
