/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONTACT_WIDGET_H
#define GADU_CONTACT_WIDGET_H

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/widgets/contact-widget.h"

class GaduContact;

class GaduContactWidget : public ContactWidget
{
	Q_OBJECT

	Contact Data;
	void createGui();

public:
	explicit GaduContactWidget(Contact contact, QWidget *parent = 0);
	virtual ~GaduContactWidget();

};

#endif // GADU_CONTACT_WIDGET_H
