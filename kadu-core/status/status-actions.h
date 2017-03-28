/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"
#include "status/status-type.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class IconsManager;
class StatusContainerManager;
class StatusContainer;
class StatusSetter;
class StatusTypeData;
class StatusTypeManager;

class QAction;
class QActionGroup;

class KADUAPI StatusActions : public QObject
{
    Q_OBJECT

public:
    explicit StatusActions(
        StatusContainer *statusContainer, bool includePrefix, bool onlyStatuses, QObject *parent = nullptr);
    virtual ~StatusActions();

    const QList<QAction *> &actions() const
    {
        return Actions;
    }
    StatusContainer *statusContainer() const
    {
        return MyStatusContainer;
    }

signals:
    void statusActionsRecreated();

    void statusActionTriggered(QAction *);
    void changeDescriptionActionTriggered(bool);

private:
    QPointer<IconsManager> m_iconsManager;
    QPointer<StatusContainerManager> m_statusContainerManager;
    QPointer<StatusSetter> m_statusSetter;
    QPointer<StatusTypeManager> m_statusTypeManager;

    StatusContainer *MyStatusContainer;
    bool IncludePrefix;
    bool OnlyStatuses;

    QActionGroup *ChangeStatusActionGroup;
    QAction *ChangeDescription;
    QList<QAction *> Actions;
    QList<StatusType> MyStatusTypes;

    void createActions();
    void createBasicActions();
    QAction *createSeparator();
    QAction *createStatusAction(const StatusTypeData &typeData);

    void cleanUpActions();

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
    INJEQT_SET void setStatusSetter(StatusSetter *statusSetter);
    INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
    INJEQT_INIT void init();

    void statusUpdated(StatusContainer *container = 0);
    void iconThemeChanged();
};
