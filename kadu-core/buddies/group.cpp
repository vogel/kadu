/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "group-manager.h"

#include "group.h"

KaduSharedBaseClassImpl(Group)

Group Group::null;

Group Group::create()
{
	return new GroupShared();
}

Group Group::loadStubFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint)
{
	return GroupShared::loadStubFromStorage(contactStoragePoint);
}

Group Group::loadFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint)
{
	return GroupShared::loadFromStorage(contactStoragePoint);
}

Group::Group()
{
}

Group::Group(GroupShared *data) :
		SharedBase<GroupShared>(data)
{
}

Group::Group(QObject *data)
{
	GroupShared *shared = qobject_cast<GroupShared *>(data);
	if (shared)
		setData(shared);
}

Group::Group(const Group&copy) :
		SharedBase<GroupShared>(copy)
{
}

Group::~Group()
{
}

KaduSharedBase_PropertyDefCRW(Group, QString, name, Name, QString())
KaduSharedBase_PropertyDefCRW(Group, QString, icon, Icon, QString())
KaduSharedBase_PropertyDef(Group, bool, notifyAboutStatusChanges, NotifyAboutStatusChanges, false)
KaduSharedBase_PropertyDef(Group, bool, showInAllGroup, ShowInAllGroup, false)
KaduSharedBase_PropertyDef(Group, bool, offlineToGroup, OfflineToGroup, false)
KaduSharedBase_PropertyDef(Group, bool, showIcon, ShowIcon, false)
KaduSharedBase_PropertyDef(Group, bool, showName, ShowName, false)
KaduSharedBase_PropertyDef(Group, int, tabPosition, TabPosition, -1)
