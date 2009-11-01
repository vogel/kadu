/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SELECT_CONTACT_COMBOBOX_H
#define SELECT_CONTACT_COMBOBOX_H

#include <QtGui/QComboBox>

#include "buddies/buddy.h"

class AbstractBuddyFilter;
class ContactsModelProxy;
class SelectContactPopup;

class SelectContactCombobox : public QComboBox
{
	Q_OBJECT

	Buddy CurrentContact;
	ContactsModelProxy *ProxyModel;
	SelectContactPopup *Popup;

private slots:
	void contactTextChanged(const QString &);

protected:
	virtual void showPopup();
	virtual void hidePopup();

public:
	explicit SelectContactCombobox(QWidget *parent = 0);
	virtual ~SelectContactCombobox();

	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	Buddy contact() { return CurrentContact; }

signals:
	void contactChanged(Buddy contact);

};

#endif // SELECT_CONTACT_COMBOBOX_H
