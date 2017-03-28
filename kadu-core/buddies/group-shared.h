/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <injeqt/injeqt.h>

#include "storage/shared.h"

class Configuration;
class GroupManager;

class KADUAPI GroupShared : public Shared
{
    Q_OBJECT

public:
    explicit GroupShared(const QUuid &uuid = QUuid());
    virtual ~GroupShared();

    virtual StorableObject *storageParent();
    virtual QString storageNodeName();

    virtual void aboutToBeRemoved();

    KaduShared_PropertyRead(const QString &, name, Name) void setName(const QString &name);

    KaduShared_Property(const QString &, icon, Icon)
        KaduShared_Property(bool, notifyAboutStatusChanges, NotifyAboutStatusChanges)
            KaduShared_Property(bool, showInAllGroup, ShowInAllGroup)
                KaduShared_Property(bool, offlineToGroup, OfflineToGroup) KaduShared_Property(bool, showIcon, ShowIcon)
                    KaduShared_Property(bool, showName, ShowName) KaduShared_Property(int, tabPosition, TabPosition)

                        signals : void updated();
    void nameChanged();
    void groupAboutToBeRemoved();

protected:
    virtual void load();
    virtual void store();

private:
    QPointer<Configuration> m_configuration;
    QPointer<GroupManager> m_groupManager;

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

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setGroupManager(GroupManager *groupManager);
};
