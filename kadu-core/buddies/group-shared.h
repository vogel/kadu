/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_SHARED_H
#define GROUP_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "status/status.h"

#include "shared/shared.h"

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
	virtual void emitUpdated();

public:
	static GroupShared * loadFromStorage(StoragePoint *groupStoragePoint);

	explicit GroupShared(QUuid uuid = QUuid());
	virtual ~GroupShared();

	virtual void load();
	virtual void store();

	KaduShared_Property(QString, name, Name)
	KaduShared_Property(QString, icon, Icon)
	KaduShared_Property(bool, notifyAboutStatusChanges, NotifyAboutStatusChanges)
	KaduShared_Property(bool, showInAllGroup, ShowInAllGroup)
	KaduShared_Property(bool, offlineToGroup, OfflineToGroup)
	KaduShared_Property(bool, showIcon, ShowIcon)
	KaduShared_Property(bool, showName, ShowName)
	KaduShared_Property(int, tabPosition, TabPosition)

signals:
	void updated();

};

#endif // GROUP_SHARED_H
