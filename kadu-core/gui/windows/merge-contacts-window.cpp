/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

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
	QHBoxLayout *layout = new QHBoxLayout(this);

	QLabel *description = new QLabel(tr("<i>Choose which contact would you like to merge with <b>%1</b></i>").arg(MyContact.display()), this);
	layout->addWidget(description);
}
