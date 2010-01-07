/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QDebug>

#include "buddies/group-manager.h"
#include "buddies/model/groups-model.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"

#include "groups-combo-box.h"

GroupsComboBox::GroupsComboBox(bool includeSelectGroups, QWidget *parent) :
		QComboBox(parent)
{
	Model = new GroupsModel(this);
	ProxyModel = new QSortFilterProxyModel(this);
	ProxyModel->setSourceModel(Model);
	ProxyModel->setDynamicSortFilter(true);
	ProxyModel->sort(1);
	ProxyModel->sort(0);

	ActionsProxyModel::ModelActionList groupsModelBeforeActions;
	if (includeSelectGroups)
		groupsModelBeforeActions.append(qMakePair<QString, QString>(tr(" - Select group - "), ""));
	ActionsProxyModel::ModelActionList groupsModelAfterActions;
	if (includeSelectGroups)
		groupsModelAfterActions.append(qMakePair<QString, QString>(tr("Create a new group..."), "createNewGroup"));
	ActionsModel = new ActionsProxyModel(groupsModelBeforeActions,
			groupsModelAfterActions, this);
	ActionsModel->setSourceModel(ProxyModel);

	setModel(ActionsModel);

	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot(int)));

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

GroupsComboBox::~GroupsComboBox()
{
}

void GroupsComboBox::setCurrentGroup(Group group)
{
	QModelIndex index = Model->groupModelIndex(group);
	index = ProxyModel->mapFromSource(index);
	index = ActionsModel->mapFromSource(index);

	if (index.row() < 0 || index.row() >= count())
		setCurrentIndex(0);
	else
		setCurrentIndex(index.row());

	CurrentGroup = group;
}

Group GroupsComboBox::currentGroup()
{
	QModelIndex index = ActionsModel->index(currentIndex(), 0);
	CurrentGroup = index.data(GroupRole).value<Group>();
	return CurrentGroup;
}

void GroupsComboBox::activatedSlot(int index)
{
	QModelIndex modelIndex = this->model()->index(index, 0, QModelIndex());
	QString action = modelIndex.data(ActionRole).toString();

	if (action.isEmpty())
		return;
	bool ok;

	QString newGroupName = QInputDialog::getText(this, tr("New Group"),
			tr("Please enter the name for the new group:"), QLineEdit::Normal,
			QString::null, &ok);

	if (!ok || newGroupName.isEmpty() || !GroupManager::instance()->acceptableGroupName(newGroupName))
	{
		setCurrentIndex(0);
		return;
	}

	GroupManager::instance()->byName(newGroupName);
	setCurrentIndex(findText(newGroupName));
}

void GroupsComboBox::resetGroup()
{
	setCurrentGroup(CurrentGroup);
}
