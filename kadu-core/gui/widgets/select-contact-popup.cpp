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

#include "select-contact-popup.h"

SelectContactPopup::SelectContactPopup(QWidget *parent) :
		ContactsListView(0, parent)
{
	setModel(new ContactsModel(ContactManager::instance(), this));
}

SelectContactPopup::~SelectContactPopup()
{
}
