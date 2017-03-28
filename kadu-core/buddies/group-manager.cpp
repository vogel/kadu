/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtXml/QDomElement>

#include "buddies/buddy-manager.h"
#include "buddies/group-storage.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "icons/icons-manager.h"
#include "storage/storage-point.h"
#include "windows/message-dialog.h"

#include "group-manager.h"

GroupManager::GroupManager(QObject *parent) : Manager<Group>{parent}
{
}

GroupManager::~GroupManager()
{
}

void GroupManager::setConfigurationManager(ConfigurationManager *configurationManager)
{
    m_configurationManager = configurationManager;
}

void GroupManager::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void GroupManager::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void GroupManager::setGroupStorage(GroupStorage *groupStorage)
{
    m_groupStorage = groupStorage;
}

void GroupManager::init()
{
    m_configurationManager->registerStorableObject(this);
}

void GroupManager::done()
{
    m_configurationManager->unregisterStorableObject(this);
}

void GroupManager::importConfiguration()
{
    QMutexLocker locker(&mutex());

    auto sp = storage();
    if (!sp || !sp->storage())
        return;

    QSet<QString> groups;
    ConfigurationApi *configurationStorage = sp->storage();

    QDomElement contactsNode = configurationStorage->getNode("Contacts", ConfigurationApi::ModeFind);
    if (contactsNode.isNull())
        return;

    QVector<QDomElement> contactsElements = configurationStorage->getNodes(contactsNode, "Contact");
    foreach (const QDomElement &contactElement, contactsElements)
        foreach (const QString &newGroup, contactElement.attribute("groups").split(',', QString::SkipEmptyParts))
            groups << newGroup;

    foreach (const QString &groupName, groups)
        byName(groupName);   // it can do import, too
}

void GroupManager::load()
{
    QMutexLocker locker(&mutex());

    QDomElement groupsNode = m_configuration->api()->getNode("Groups", ConfigurationApi::ModeFind);
    if (groupsNode.isNull())
    {
        importConfiguration();
        setState(StateLoaded);
        return;
    }

    Manager<Group>::load();
}

void GroupManager::store()
{
    QMutexLocker locker(&mutex());

    emit saveGroupData();

    Manager<Group>::store();
}

Group GroupManager::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    return m_groupStorage->loadStubFromStorage(storagePoint);
}

Group GroupManager::byName(const QString &name, bool create)
{
    QMutexLocker locker(&mutex());

    if (name.isEmpty())
        return Group::null;

    ensureLoaded();

    foreach (Group group, items())
        if (name == group.name())
            return group;

    if (!create)
        return Group::null;

    auto group = m_groupStorage->create();
    group.data()->importConfiguration(name);
    addItem(group);

    return group;
}

QString GroupManager::validateGroupName(Group group, const QString &newName)
{
    if (newName.isEmpty())
        return tr("Group name must not be empty");

    if (newName.contains(","))
        return tr("Group name must not contain '%1'").arg(',');

    if (newName.contains(";"))
        return tr("Group name must not contain '%1'").arg(';');

    bool number;
    newName.toLong(&number);
    if (number)
        return tr("Group name must not be a number");

    // TODO All translations
    if (newName == tr("All"))
        return tr("Group name must not be '%1'").arg(newName);

    auto existing = byName(newName, false);
    if (existing && existing != group)
        return tr("Group '%1' already exists").arg(newName);

    return QString{};
}

// TODO: move some of this to %like-encoding, so we don't block normal names
bool GroupManager::acceptableGroupName(const QString &groupName, bool acceptExistingGroupName)
{
    if (groupName.isEmpty())
    {
        return false;
    }

    if (groupName.contains(","))
    {
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("'%1' is prohibited").arg(','));
        return false;
    }

    if (groupName.contains(";"))
    {
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("'%1' is prohibited").arg(';'));
        return false;
    }

    bool number;
    groupName.toLong(&number);
    if (number)
    {
        // because of gadu-gadu contact list format...
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("Numbers are prohibited"));
        return false;
    }

    // TODO All translations
    if (groupName == tr("All"))
    {
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
            tr("Group name %1 is prohibited").arg(groupName));
        return false;
    }

    if (!acceptExistingGroupName && byName(groupName, false))
    {
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
            tr("Group of that name already exists!"));
        return false;
    }

    return true;
}

void GroupManager::groupDataUpdated()
{
    Group group(sender());
    if (!group.isNull())
        emit groupUpdated(group);
}

void GroupManager::itemAboutToBeAdded(Group item)
{
    connect(item, SIGNAL(updated()), this, SLOT(groupDataUpdated()));
    emit groupAboutToBeAdded(item);
}

void GroupManager::itemAdded(Group item)
{
    emit groupAdded(item);
}

void GroupManager::itemAboutToBeRemoved(Group item)
{
    emit groupAboutToBeRemoved(item);
}

void GroupManager::itemRemoved(Group item)
{
    disconnect(item, 0, this, 0);
    emit groupRemoved(item);
}

#include "moc_group-manager.cpp"
