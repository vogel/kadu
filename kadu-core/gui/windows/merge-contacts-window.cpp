/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/filter/contact-mergable-filter.h"
#include "gui/widgets/select-contact-combobox.h"
#include "icons-manager.h"

#include "merge-contacts-window.h"

MergeContactsWindow::MergeContactsWindow(Buddy contact, QWidget * parent) :
		QDialog(parent), MyContact(contact)
{
	createGui();
}

MergeContactsWindow::~MergeContactsWindow()
{
}

void MergeContactsWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	layout->addWidget(new QLabel(tr("<i>Choose which contact would you like to merge with <b>%1</b></i>")
			.arg(MyContact.display()), this));

	QWidget *chooseWidget = new QWidget(this);
	layout->addWidget(chooseWidget);

	QHBoxLayout *chooseLayout = new QHBoxLayout(chooseWidget);

	chooseLayout->addWidget(new QLabel(tr("Contact:"), this));
	SelectCombo = new SelectContactCombobox(this);
	SelectCombo->addFilter(new ContactMergableFilter(MyContact, SelectCombo));
	connect(SelectCombo, SIGNAL(contactChanged(Buddy)), this, SLOT(selectedContactChanged(Buddy)));
	chooseLayout->addWidget(SelectCombo);

	layout->addStretch(100);
	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons);

	MergeButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Merge"), this);
	MergeButton->setDefault(true);
	MergeButton->setEnabled(false);
	connect(MergeButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(MergeButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);
}

void MergeContactsWindow::selectedContactChanged(Buddy contact)
{
	MergeButton->setEnabled(!contact.isNull());
}

void MergeContactsWindow::accept()
{
	Buddy mergeWith = SelectCombo->contact();
	if (mergeWith.isNull())
		return;

	if (MyContact.isNull())
		return;

	BuddyManager::instance()->mergeBuddies(mergeWith, MyContact);

	QDialog::accept();
}
