/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/group.h"
#include "storage/simple-manager.h"
#include "exports.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <injeqt/injeqt.h>

class ConfigurationManager;
class Configuration;
class IconsManager;
class GroupStorage;

class KADUAPI GroupManager : public SimpleManager<Group>
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit GroupManager(QObject *parent = nullptr);
	virtual ~GroupManager();

	virtual QString storageNodeName() { return QLatin1String("Groups"); }
	virtual QString storageNodeItemName() { return QLatin1String("Group"); }

	Group byName(const QString &name, bool create = true);

	QString validateGroupName(Group group, const QString &newName);
	bool acceptableGroupName(const QString &groupName, bool acceptExistingGroupName = false);

signals:
	void groupAboutToBeAdded(Group group);
	void groupAdded(Group group);
	void groupAboutToBeRemoved(Group group);
	void groupRemoved(Group group);

	void groupUpdated(Group group);

	void saveGroupData();

protected:
	virtual void load();
	virtual void store();
	virtual Group loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint) override;

	virtual void itemAboutToBeAdded(Group item);
	virtual void itemAdded(Group item);
	virtual void itemAboutToBeRemoved(Group item);
	virtual void itemRemoved(Group item);

private:
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;
	QPointer<GroupStorage> m_groupStorage;

	void importConfiguration();

private slots:
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setGroupStorage(GroupStorage *groupStorage);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void groupDataUpdated();

};
