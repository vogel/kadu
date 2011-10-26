/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QAction>
#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QSortFilterProxyModel>

#include "buddies/group-manager.h"
#include "buddies/model/groups-model.h"
#include "model/roles.h"

#include "groups-combo-box.h"

GroupsComboBox::GroupsComboBox(QWidget *parent) :
		ActionsComboBox(parent), InActivatedSlot(false)
{
	QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
	setDataRole(GroupRole);
	setUpModel(new GroupsModel(this), proxyModel);

	addBeforeAction(new QAction(tr(" - Select group - "), this));

	proxyModel->setDynamicSortFilter(true);
	proxyModel->sort(0);

	CreateNewGroupAction = new QAction(tr("Create a new group..."), this);
	QFont createNewGroupActionFont = CreateNewGroupAction->font();
	createNewGroupActionFont.setItalic(true);
	CreateNewGroupAction->setFont(createNewGroupActionFont);
	CreateNewGroupAction->setData("createNewGroup");

	addAfterAction(CreateNewGroupAction);

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot(int)));
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

GroupsComboBox::~GroupsComboBox()
{
}

void GroupsComboBox::setCurrentGroup(Group group)
{
	setCurrentValue(group);
}

Group GroupsComboBox::currentGroup()
{
	return currentValue().value<Group>();
}

void GroupsComboBox::resetComboBox()
{
	if (!InActivatedSlot)
		setCurrentIndex(0);
}

void GroupsComboBox::activatedSlot(int index)
{
	InActivatedSlot = true;

	QModelIndex modelIndex = model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();

	if (action == CreateNewGroupAction)
	{
		bool ok;

		QString newGroupName = QInputDialog::getText(this, tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString(), &ok);

		if (ok)
		{
			ok = GroupManager::instance()->acceptableGroupName(newGroupName, true);

			Group switchToGroup = GroupManager::instance()->byName(newGroupName, ok);
			if (switchToGroup)
				setCurrentGroup(switchToGroup);
			else
				setCurrentIndex(0);
		}
		else
			setCurrentValue(CurrentValue);
	}

	InActivatedSlot = false;
}

void GroupsComboBox::currentIndexChangedSlot(int index)
{
	QModelIndex modelIndex = model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();
	if (action == CreateNewGroupAction)
	{
		// this is needed to fix bugs #1674 and #1690
		// as this action has to be activated by the user, otherwise we have to ignore it and reset combo box
		// TODO: try to redo this as this is a bit tricky
		if (!InActivatedSlot)
			QMetaObject::invokeMethod(this, "resetComboBox", Qt::QueuedConnection);
		return;
	}

	ActionsComboBox::currentIndexChangedSlot(index);
}

bool GroupsComboBox::compare(QVariant value, QVariant previousValue) const
{
	return value.value<Group>() == previousValue.value<Group>();
}
