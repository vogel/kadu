/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "misc/misc.h"

#include "jabber-contact.h"
#include "jabber-contact-data-manager.h"

#include "jabber-contact-widget.h"

JabberContactWidget::JabberContactWidget(JabberContact *contact, QWidget *parent) :
	ContactWidget(new JabberContactDataManager(contact, parent),
		contact, parent),
	Data(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

JabberContactWidget::~JabberContactWidget()
{
}

void JabberContactWidget::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/jabber_contact.ui"));
	//QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widgetById("password"));
	//passwordLineEdit->setEchoMode(QLineEdit::Password);
}
