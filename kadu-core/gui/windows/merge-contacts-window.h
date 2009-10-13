/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGE_CONTACTS_WINDOW_H
#define MERGE_CONTACTS_WINDOW_H

#include <QtGui/QDialog>

#include "contacts/contact.h"

class MergeContactsWindow : public QDialog
{
	Q_OBJECT

	Contact MyContact;

	void createGui();

public:
	explicit MergeContactsWindow(Contact contact, QWidget *parent = 0);
	virtual ~MergeContactsWindow();

};

#endif // MERGE_CONTACTS_WINDOW_H
