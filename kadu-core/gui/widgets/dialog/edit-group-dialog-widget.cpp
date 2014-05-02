/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QLineEdit>

#include "buddies/group-manager.h"

#include "edit-group-dialog-widget.h"

EditGroupDialogWidget::EditGroupDialogWidget(Group group, const QString &message, QWidget *parent) :
		AddGroupDialogWidget(message, parent), CurrentGroup(group)
{
// 	QIcon icon = KaduIcon("kadu_icons/kadu").icon();
// 	Pixmap = icon.pixmap(icon.actualSize(QSize(64, 64)));
// 
// 	createGui();
}

EditGroupDialogWidget::~EditGroupDialogWidget()
{
}

void EditGroupDialogWidget::dialogAccepted()
{
	QString newGroupName = GroupName->text();
	if (!newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		CurrentGroup.setName(newGroupName);
}

#include "moc_edit-group-dialog-widget.cpp"
