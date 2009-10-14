/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

#include "contacts/filter/contact-name-filter.h"
#include "gui/widgets/contacts-list-view.h"

#include "contacts-list-widget.h"

ContactsListWidget::ContactsListWidget(MainWindow *mainWindow, QWidget *parent) :
		QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QWidget *topWidget = new QWidget(this);
	layout->addWidget(topWidget);

	QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
	topLayout->setMargin(0);

	NameFilterEdit = new QLineEdit(this);
	topLayout->addWidget(NameFilterEdit);
	connect(NameFilterEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(nameFilterChanged(const QString &)));

	View = new ContactsListView(mainWindow, this);
	layout->addWidget(View);

	NameFilter = new ContactNameFilter(this);
	View->addFilter(NameFilter);
}

ContactsListWidget::~ContactsListWidget()
{
}

void ContactsListWidget::nameFilterChanged(const QString &filter)
{
	NameFilter->setName(filter);
}
