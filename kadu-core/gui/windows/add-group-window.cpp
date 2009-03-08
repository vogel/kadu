/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "QtGui/QDialogButtonBox"
#include "QtGui/QLabel"
#include "QtGui/QLineEdit"
#include "QtGui/QPushButton"
#include "QtGui/QVBoxLayout"

#include "contacts/group-manager.h"
#include "icons_manager.h"

#include "add-group-window.h"

AddGroupWindow::AddGroupWindow(QWidget *parent)
	: QWidget(parent, Qt::Dialog), contacts(ContactList())
{
	createGui();
}

AddGroupWindow::AddGroupWindow(ContactList &contactsToAdd, QWidget *parent)
	: QWidget(parent, Qt::Dialog), contacts(contactsToAdd)
{
	createGui();
}

void AddGroupWindow::createGui()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("New Group"));

	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QLabel *info = new QLabel;
	info->setText(tr("Please enter the name for the new group:"));
#ifndef Q_OS_MAC
	info->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
#endif

	edit = new QLineEdit(this);
	connect(edit, SIGNAL(returnPressed()), this, SLOT(okClicked()));
	connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));

	QDialogButtonBox *buttons_layout = new QDialogButtonBox(Qt::Horizontal, this);

	okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), this);
	buttons_layout->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	main_layout->addWidget(info);
	main_layout->addWidget(edit);
	main_layout->addSpacing(10);//TODO
	main_layout->addWidget(buttons_layout);
}

void AddGroupWindow::okClicked()
{
	QString newGroupName = edit->text();

	if (!GroupManager::instance()->acceptableGroupName(newGroupName))
		return;

	Group *group = GroupManager::instance()->byName(newGroupName, true);

	foreach (Contact contact, contacts)
		contact.addToGroup(group);

	close();
}
void AddGroupWindow::textChanged(const QString &groupName)
{
	okButton->setEnabled(!groupName.isEmpty());
}
