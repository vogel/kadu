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

#include "gui/widgets/select-contact-combobox.h"

#include "merge-contacts-window.h"

MergeContactsWindow::MergeContactsWindow(Contact contact, QWidget * parent) :
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
	SelectContactCombobox* selectCombo = new SelectContactCombobox(this);
	connect(selectCombo, SIGNAL(contactChanged(Contact)), this, SLOT(selectedContactChanged(Contact)));
	chooseLayout->addWidget(selectCombo);

	layout->addStretch(100);
	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons);

	QPushButton *mergeButton = new QPushButton(tr("Merge"), this);
	mergeButton->setDefault(true);

	QPushButton *cancel = new QPushButton(tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(mergeButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);
}

void MergeContactsWindow::selectedContactChanged(Contact contact)
{
	printf("contact changed: %s\n", qPrintable(contact.display()));
}
