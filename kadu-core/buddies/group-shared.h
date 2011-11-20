/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GROUP_SHARED_H
#define GROUP_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "storage/shared.h"

class KADUAPI GroupShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(GroupShared)

	QString Name;
	QString Icon;
	bool NotifyAboutStatusChanges;
	bool ShowInAllGroup;
	bool OfflineToGroup;
	bool ShowIcon;
	bool ShowName;
	int TabPosition;

	friend class GroupManager;
	void importConfiguration(const QString &name);

protected:
	virtual void load();
	virtual void store();
	virtual void emitUpdated();

public:
	static GroupShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &groupStoragePoint);
	static GroupShared * loadFromStorage(const QSharedPointer<StoragePoint> &groupStoragePoint);

	explicit GroupShared(const QUuid &uuid = QUuid());
	virtual ~GroupShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	KaduShared_PropertyRead(const QString &, name, Name)
	void setName(const QString &name);

	KaduShared_Property(const QString &, icon, Icon)
	KaduShared_Property(bool, notifyAboutStatusChanges, NotifyAboutStatusChanges)
	KaduShared_Property(bool, showInAllGroup, ShowInAllGroup)
	KaduShared_Property(bool, offlineToGroup, OfflineToGroup)
	KaduShared_Property(bool, showIcon, ShowIcon)
	KaduShared_Property(bool, showName, ShowName)
	KaduShared_Property(int, tabPosition, TabPosition)

signals:
	void updated();
	void nameChanged();
	void groupAboutToBeRemoved();

};

#endif // GROUP_SHARED_H
