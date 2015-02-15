/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/group-manager.h"

#include "group-list.h"

GroupList::GroupList(QWidget *parent) :
		QListWidget(parent)
{
	foreach (const Group &group, GroupManager::instance()->items())
	{
		QListWidgetItem * const groupItem = new QListWidgetItem(this);
		groupItem->setText(group.name());
	}
}

GroupList::~GroupList()
{
}

void GroupList::setCheckedGroups(const QSet<Group> groups)
{
	QSet<QString> checkedNames;
	foreach (const Group &group, groups)
		checkedNames.insert(group.name());

	const int itemCount = count();
	for (int i = 0; i < itemCount; i++)
	{
		QListWidgetItem * const groupItem = item(i);
		if (checkedNames.contains(groupItem->text()))
			groupItem->setCheckState(Qt::Checked);
		else
			groupItem->setCheckState(Qt::Unchecked);
	}
}

QSet<Group> GroupList::checkedGroups()
{
	QSet<Group> result;

	const int itemCount = count();
	for (int i = 0; i < itemCount; i++)
	{
		const QListWidgetItem * const groupItem = item(i);
		if (Qt::Checked == groupItem->checkState())
			result.insert(GroupManager::instance()->byName(groupItem->text()));
	}

	return result;
}

#include "moc_group-list.cpp"
