/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/model/contacts-model.h"

#include "contacts-list-widget-delegate.h"

#include "contacts-list-widget.h"

ContactsListWidget::ContactsListWidget(QWidget *parent)
	: QListView(parent)
{
	Delegate = new ContactsListWidgetDelegate();

	setModel(new ContactsModel(this));
//	setItemDelegate(Delegate);
}

ContactsListWidget::~ContactsListWidget()
{
	if (Delegate)
	{
		delete Delegate;
		Delegate = 0;
	}
}

