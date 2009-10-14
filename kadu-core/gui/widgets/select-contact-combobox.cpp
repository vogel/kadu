/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/widgets/select-contact-popup.h"

#include "select-contact-combobox.h"

SelectContactCombobox::SelectContactCombobox(QWidget *parent) :
		QComboBox(parent)
{
	Popup = new SelectContactPopup();
	Popup->hide();
}

SelectContactCombobox::~SelectContactCombobox()
{
	delete Popup;
	Popup = 0;
}

void SelectContactCombobox::showPopup()
{
	Popup->show();
}

void SelectContactCombobox::hidePopup()
{
	Popup->hide();
}
