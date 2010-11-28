/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "buddies/group.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "storage/storage-point.h"

#include "debug.h"

#include "group-manager.h"

GroupManager *GroupManager::Instance = 0;

GroupManager * GroupManager::instance()
{
	if (!Instance)
		Instance = new GroupManager();

	return Instance;
}

GroupManager::GroupManager()
{
	ConfigurationManager::instance()->registerStorableObject(this);
}

GroupManager::~GroupManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void GroupManager::importConfiguration()
{
	QMutexLocker(&mutex());

	QSharedPointer<StoragePoint> sp(storage());
	if (!sp || !sp->storage())
		return;

	QSet<QString> groups;
	XmlConfigFile *configurationStorage = sp->storage();

	QDomElement contactsNode = configurationStorage->getNode("Contacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QList<QDomElement> contactsElements = configurationStorage->getNodes(contactsNode, "Contact");
	foreach (QDomElement contactElement, contactsElements)
		foreach (QString newGroup, contactElement.attribute("groups").split(',', QString::SkipEmptyParts))
			groups << newGroup;

	foreach (QString groupName, groups)
		byName(groupName); // it can do import, too
}

void GroupManager::load()
{
	QMutexLocker(&mutex());

	QDomElement groupsNode = xml_config_file->getNode("Groups", XmlConfigFile::ModeFind);
	if (groupsNode.isNull())
	{
		importConfiguration();
		setState(StateLoaded);
		return;
	}

	SimpleManager<Group>::load();
}

void GroupManager::store()
{
	QMutexLocker(&mutex());

	emit saveGroupData();

	SimpleManager<Group>::store();
}

Group GroupManager::byName(const QString &name, bool create)
{
	QMutexLocker(&mutex());

	if (name.isEmpty())
		return Group::null;

	ensureLoaded();

	foreach (Group group, items())
		if (name == group.name())
			return group;

	if (!create)
		return Group::null;

	Group group = Group::create();
	group.data()->importConfiguration(name);
	addItem(group);

	return group;
}

// TODO: move some of this to %like-encoding, so we don't block normal names
bool GroupManager::acceptableGroupName(const QString &groupName, bool acceptExistingGroupName)
{
	kdebugf();
	if (groupName.isEmpty())
	{
		kdebugf2();
		return false;
	}

	if (groupName.contains(","))
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("'%1' is prohibited").arg(','));
		kdebugf2();
		return false;
	}

	if (groupName.contains(";"))
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("'%1' is prohibited").arg(';'));
		kdebugf2();
		return false;
	}

	bool number;
	groupName.toLong(&number);
	if (number)
	{
		// because of gadu-gadu contact list format...
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Numbers are prohibited"));
		kdebugf2();
		return false;
	}

	// TODO All translations
 	if (groupName == tr("All"))
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Group name %1 is prohibited").arg(groupName));
 		kdebugf2();
 		return false;
	}

	if (!acceptExistingGroupName && byName(groupName, false))
 	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Group of that name already exists!"));
 		kdebugf2();
 		return false;
 	}

	kdebugf2();
	return true;
}

void GroupManager::itemAboutToBeAdded(Group item)
{
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
	emit groupRemoved(item);
}
