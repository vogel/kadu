/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "accounts/model/accounts-model.h"
#include "contacts/model/groups-model.h"
#include "misc/misc.h"

#include "add-buddy-window.h"

AddBuddyWindow::AddBuddyWindow(QWidget *parent) :
		QDialog(parent, Qt::Window)
{
	createGui();
}

AddBuddyWindow::~AddBuddyWindow()
{
	saveWindowGeometry(this, "General", "AddBuddyWindowGeometry");
}

void AddBuddyWindow::createGui()
{
	loadWindowGeometry(this, "General", "AddBuddyWindowGeometry", 0, 50, 425, 430);

	QGridLayout *layout = new QGridLayout(this);

	layout->addWidget(new QLabel(tr("Username:"), this), 0, 0, Qt::AlignRight);
	UserNameEdit = new QLineEdit(this);
	layout->addWidget(UserNameEdit, 0, 1);
	layout->addWidget(new QLabel(tr("in"), this), 0, 2);

	AccountCombo = new QComboBox(this);
	AccountCombo->setModel(new AccountsModel(AccountCombo));
	layout->addWidget(AccountCombo, 0, 3);

	layout->addWidget(new QLabel(tr("Add in group:"), this), 1, 0, Qt::AlignRight);
	GroupCombo = new QComboBox(this);
	GroupCombo->setModel(new GroupsModel(GroupCombo));
	layout->addWidget(GroupCombo, 1, 1, 1, 3);

	layout->addWidget(new QLabel(tr("Visible name:"), this), 2, 0, Qt::AlignRight);
	DisplayNameEdit = new QLineEdit(this);
	layout->addWidget(DisplayNameEdit, 2, 1, 1, 3);

	QLabel *hintLabel = new QLabel(tr("Enter a name for this contact"));
	QFont hintLabelFont = hintLabel->font();
	hintLabelFont.setItalic(true);
	hintLabelFont.setPointSize(hintLabelFont.pointSize() - 1);
	hintLabel->setFont(hintLabelFont);
	layout->addWidget(hintLabel, 3, 1, 1, 3);

	AllowToSeeMeCheck = new QCheckBox(tr("Allow contact to see me when I'm available"), this);
	AllowToSeeMeCheck->setChecked(true);
	layout->addWidget(AllowToSeeMeCheck, 5, 1, 1, 3);

	layout->setRowMinimumHeight(4, 20);
	layout->setRowMinimumHeight(6, 20);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons, 7, 0, 1, 4);

	QPushButton *addContact = new QPushButton(tr("Add contact"), this);
	QPushButton *cancel = new QPushButton(tr("Cancel"), this);

	buttons->addButton(addContact, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);

// 	TODO: NOW, does not work
// 	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
// 	setMaximumHeight(layout->minimumSize().height());
// 	layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
}
