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

class SelectContactPopup;

class SelectContactCombobox : public QComboBox
{
	Q_OBJECT

	SelectContactPopup *Popup;

protected:
	virtual void showPopup();
	virtual void hidePopup();

public:
	explicit SelectContactCombobox(QWidget *parent = 0);
	virtual ~SelectContactCombobox();

};

#endif // SELECT_CONTACT_COMBOBOX_H
