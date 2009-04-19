/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_H
#define GROUP_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/uuid-storable-object.h"

class Group : public QObject, public UuidStorableObject
{
friend class GroupManager;

	Q_OBJECT

	QUuid Uuid;
	QString Name;
	QString Icon;
	bool NotifyAboutStatusChanges;
	bool ShowInAllGroup;
	bool OfflineToGroup;
	bool ShowIcon;
	bool ShowName;

	void importConfiguration(const QString &name);

public:
	static Group * loadFromStorage(StoragePoint *groupStoragePoint);

	explicit Group(StoragePoint *storagePoint);
	explicit Group(QUuid uuid = QUuid());
	~Group();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }

	QString name() const { return Name; }
	QString icon() const { return Icon; }
	bool showIcon() const { return ShowIcon; }
	bool showName() const { return ShowName; }
	bool offlineToGroup() const { return OfflineToGroup; }
	bool showInAllGroup() const { return ShowInAllGroup; }
	bool notifyAboutStatusChanges() const { return NotifyAboutStatusChanges; }
	

	void setName(const QString &name);
	void setAppearance(bool showName, bool showIcon, const QString &icon);
	void setNotifyAboutStatuses(bool notify);
	void setOfflineTo(bool offline);
	void setShowInAllGroup(bool show);

signals:
	void appearanceChanged(const Group *group);
	void nameChanged(const Group *group);
	void showInAllChanged();

};

#endif // GROUP_H
