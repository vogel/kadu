/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "widgets/actions-combo-box.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class GroupManager;

class GroupsComboBox : public ActionsComboBox
{
    Q_OBJECT

public:
    explicit GroupsComboBox(QWidget *parent = nullptr);
    virtual ~GroupsComboBox();

    void setCurrentGroup(Group group);
    Group currentGroup();

private:
    QPointer<GroupManager> m_groupManager;

    QAction *m_createNewGroupAction;

private slots:
    INJEQT_SET void setGroupManager(GroupManager *groupManager);
    INJEQT_INIT void init();

    void createNewGroup();
};
