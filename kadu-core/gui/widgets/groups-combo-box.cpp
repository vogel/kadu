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
		KaduComboBox<Group>(parent), InActivatedSlot(false)
{
	setUpModel(new GroupsModel(this), new QSortFilterProxyModel(this));

	static_cast<QSortFilterProxyModel *>(SourceProxyModel)->setDynamicSortFilter(true);
	SourceProxyModel->sort(0);

	CreateNewGroupAction = new QAction(tr("Create a new group..."), this);
	CreateNewGroupAction->setData("createNewGroup");

	ActionsModel->addAfterAction(CreateNewGroupAction);

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
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
	return currentValue();
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

		ok = ok && GroupManager::instance()->acceptableGroupName(newGroupName, true);

		Group switchToGroup = GroupManager::instance()->byName(newGroupName, ok);
		if (switchToGroup)
			setCurrentGroup(switchToGroup);
		else
			setCurrentIndex(0);
	}

	InActivatedSlot = false;
}

void GroupsComboBox::currentIndexChangedSlot(int index)
{
	QModelIndex modelIndex = model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();
	if (action == CreateNewGroupAction)
	{
		// this is needed to fix Mantis bugs #1674 and #1690
		// as this action has to be activated by the user, otherwise we have to ignore it and reset combo box
		// TODO 0.9: try to redo this as this is a bit tricky
		if (!InActivatedSlot)
			QMetaObject::invokeMethod(this, "resetComboBox", Qt::QueuedConnection);
		return;
	}

	KaduComboBox<Group>::currentIndexChangedSlot(index);
}

void GroupsComboBox::updateValueBeforeChange()
{
	KaduComboBox<Group>::updateValueBeforeChange();
}

void GroupsComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	KaduComboBox<Group>::rowsRemoved(parent, start, end);
}

int GroupsComboBox::preferredDataRole() const
{
	return GroupRole;
}

QString GroupsComboBox::selectString() const
{
	return tr(" - Select group - ");
}
