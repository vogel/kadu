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

#include <QtGui/QFormLayout>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "buddies/group-manager.h"
#include "icons/kadu-icon.h"

#include "add-group-dialog-widget.h"

AddGroupDialogWidget::AddGroupDialogWidget(const QString &message, QWidget *parent) :
		DialogWidget(tr("Add Group"), message, QPixmap(), parent)
{
	QIcon icon = KaduIcon("kadu_icons/kadu").icon();
	Pixmap = icon.pixmap(icon.actualSize(QSize(64, 64)));

	createGui();
}

AddGroupDialogWidget::~AddGroupDialogWidget()
{
}

void AddGroupDialogWidget::createGui()
{
	formLayout = new QFormLayout(this);
	formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	GroupName = new QLineEdit(this);
	GroupName->setFocus();
	connect(GroupName, SIGNAL(textChanged(const QString &)), this, SLOT(groupNameTextChanged(const QString &)));

	QLabel *addGroupLabel = new QLabel(tr("Group Name") + ":", this);
	formLayout->addRow(addGroupLabel, GroupName);

	setLayout(formLayout);
}

void AddGroupDialogWidget::groupNameTextChanged(const QString& text)
{
	emit valid(GroupManager::instance()->acceptableGroupName(text));
}

void AddGroupDialogWidget::dialogAccepted()
{
	QString newGroupName = GroupName->text();
	if (!newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
}

void AddGroupDialogWidget::dialogRejected()
{
}

#include "moc_add-group-dialog-widget.cpp"
