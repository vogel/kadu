/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyManager;
class GroupManager;
class Group;
class NotificationService;

class GroupEventListener : public QObject
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(LISTENER)

public:
    Q_INVOKABLE explicit GroupEventListener(QObject *parent = nullptr);
    virtual ~GroupEventListener();

private:
    QPointer<BuddyManager> m_buddyManager;
    QPointer<GroupManager> m_groupManager;

private slots:
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setGroupManager(GroupManager *groupManager);
    INJEQT_INIT void init();

    void groupAdded(const Group &group);
    void groupUpdated();
};
