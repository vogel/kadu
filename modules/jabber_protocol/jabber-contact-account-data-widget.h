/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CONTACT_ACCOUNT_DATA_WIDGET
#define JABBER_CONTACT_ACCOUNT_DATA_WIDGET

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/widgets/contact-account-data-widget.h"

class JabberContactAccountData;

class JabberContactAccountDataWidget : public ContactAccountDataWidget
{
	Q_OBJECT

	JabberContactAccountData *Data;
	void createGui();

public:
	explicit JabberContactAccountDataWidget(JabberContactAccountData *contactAccountData, QWidget *parent = 0);
	~JabberContactAccountDataWidget();

};

#endif // JABBER_CONTACT_ACCOUNT_DATA_WIDGET
