/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SELECT_CONTACT_POPUP_H
#define SELECT_CONTACT_POPUP_H

#include "gui/widgets/contacts-list-view.h"

class SelectContactPopup : public ContactsListView
{
	Q_OBJECT

public:
	explicit SelectContactPopup(QWidget *parent = 0);
	virtual ~SelectContactPopup();

};

#endif // SELECT_CONTACT_POPUP_H
