/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "status-actions.h"

#include "accounts/account-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol.h"
#include "status/status-container-manager.h"
#include "status/status-container.h"
#include "status/status-setter.h"
#include "status/status-type-data.h"
#include "status/status-type-group.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"

#include <QtWidgets/QAction>

StatusActions::StatusActions(StatusContainer *statusContainer, bool includePrefix, bool onlyStatuses, QObject *parent)
        : QObject(parent), MyStatusContainer(statusContainer), IncludePrefix(includePrefix), OnlyStatuses{onlyStatuses},
          ChangeDescription(0)
{
}

StatusActions::~StatusActions()
{
}

void StatusActions::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void StatusActions::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
    m_statusContainerManager = statusContainerManager;
}

void StatusActions::setStatusSetter(StatusSetter *statusSetter)
{
    m_statusSetter = statusSetter;
}

void StatusActions::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
    m_statusTypeManager = statusTypeManager;
}

void StatusActions::init()
{
    ChangeStatusActionGroup = new QActionGroup(this);
    ChangeStatusActionGroup->setExclusive(true);   // HACK
    connect(ChangeStatusActionGroup, SIGNAL(triggered(QAction *)), this, SIGNAL(statusActionTriggered(QAction *)));

    statusUpdated();
    connect(MyStatusContainer, SIGNAL(statusUpdated(StatusContainer *)), this, SLOT(statusUpdated(StatusContainer *)));

    connect(m_iconsManager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

void StatusActions::createActions()
{
    createBasicActions();

    MyStatusTypes = MyStatusContainer->supportedStatusTypes();
    StatusTypeGroup currentGroup = StatusTypeGroup::None;
    bool setDescriptionAdded = false;

    foreach (StatusType statusType, MyStatusTypes)
    {
        if (StatusType::None == statusType)
            continue;

        const StatusTypeData &typeData = m_statusTypeManager->statusTypeData(statusType);

        if (StatusTypeGroup::None == currentGroup)
            currentGroup = typeData.typeGroup();

        if (!setDescriptionAdded && typeData.typeGroup() == StatusTypeGroup::Offline)
        {
            if (!OnlyStatuses && !Actions.isEmpty())
            {
                Actions.append(createSeparator());
                Actions.append(ChangeDescription);
                setDescriptionAdded = true;
            }
        }

        if (typeData.typeGroup() != currentGroup)
        {
            if (!OnlyStatuses)
                Actions.append(createSeparator());
            currentGroup = typeData.typeGroup();
        }

        QAction *action = createStatusAction(typeData);
        Actions.append(action);
    }

    emit statusActionsRecreated();
}

void StatusActions::createBasicActions()
{
    ChangeDescription = new QAction(tr("Change Status Message..."), this);
    connect(ChangeDescription, SIGNAL(triggered(bool)), this, SIGNAL(changeDescriptionActionTriggered(bool)));
}

QAction *StatusActions::createSeparator()
{
    QAction *separator = new QAction(this);
    separator->setSeparator(true);

    return separator;
}

QAction *StatusActions::createStatusAction(const StatusTypeData &typeData)
{
    KaduIcon icon = MyStatusContainer->statusIcon(Status{typeData.type()});
    QAction *statusAction = ChangeStatusActionGroup->addAction(
        m_iconsManager->iconByPath(icon),
        IncludePrefix ? MyStatusContainer->statusNamePrefix() + typeData.displayName() : typeData.displayName());
    statusAction->setCheckable(true);
    statusAction->setData(QVariant::fromValue(typeData.type()));

    return statusAction;
}

void StatusActions::cleanUpActions()
{
    foreach (QAction *action, Actions)
        if (action != ChangeDescription)
        {
            if (!action->isSeparator())
                ChangeStatusActionGroup->removeAction(action);

            delete action;
        }

    Actions.clear();

    delete ChangeDescription;
    ChangeDescription = 0;
}

void StatusActions::statusUpdated(StatusContainer *container)
{
    if (MyStatusContainer->supportedStatusTypes() != MyStatusTypes)
    {
        cleanUpActions();
        createActions();
    }

    StatusType currentStatusType =
        container ? container->status().type() : m_statusSetter->manuallySetStatus(MyStatusContainer).type();

    if (!MyStatusContainer->supportedStatusTypes().contains(currentStatusType))
        currentStatusType = MyStatusContainer->status().type();

    foreach (QAction *action, ChangeStatusActionGroup->actions())
    {
        StatusType statusType = action->data().value<StatusType>();
        if (StatusType::None == statusType)
            continue;
        action->setIcon(m_iconsManager->iconByPath(MyStatusContainer->statusIcon(Status{statusType})));

        if (!MyStatusContainer->isStatusSettingInProgress())
        {
            // For 'All xxx' status menu items - check only if all accounts have the same status
            if (m_statusContainerManager == MyStatusContainer)
                action->setChecked(m_statusContainerManager->allStatusOfType(statusType));
            else
                action->setChecked(currentStatusType == statusType);
        }
        else
            action->setChecked(false);
    }
}

void StatusActions::iconThemeChanged()
{
    foreach (QAction *action, ChangeStatusActionGroup->actions())
    {
        StatusType statusType = action->data().value<StatusType>();
        if (StatusType::None == statusType)
            continue;

        action->setIcon(m_iconsManager->iconByPath(MyStatusContainer->statusIcon(Status{statusType})));
    }
}

#include "moc_status-actions.cpp"
