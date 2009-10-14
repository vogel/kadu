/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-manager.h"
#include "contacts/model/contacts-model.h"
#include "gui/widgets/contacts-list-view.h"

#include "select-contact-popup.h"

SelectContactPopup::SelectContactPopup(QWidget *parent) :
		ContactsListWidget(0, parent)
{
	setWindowFlags(Qt::Popup);

	ContactsModel *model = new ContactsModel(ContactManager::instance(), this);

	view()->setItemsExpandable(false);
	view()->setModel(model);
}

SelectContactPopup::~SelectContactPopup()
{
}
