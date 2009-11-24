/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CONTACT_WIDGET_H
#define JABBER_CONTACT_WIDGET_H

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/widgets/contact-widget.h"

class Contact;
class JabberContactDetails;

class JabberContactWidget : public ContactWidget
{
	Q_OBJECT

	JabberContactDetails *Data;
	void createGui();

public:
	explicit JabberContactWidget(Contact contact, QWidget *parent = 0);
	virtual ~JabberContactWidget();

};

#endif // JABBER_CONTACT_WIDGET_H
