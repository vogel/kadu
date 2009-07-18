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

#include "jabber-contact-account-data.h"
#include "jabber-contact-account-data-manager.h"
#include "misc/misc.h"

#include "jabber-contact-account-data-widget.h"

JabberContactAccountDataWidget::JabberContactAccountDataWidget(JabberContactAccountData *contactAccountData, QWidget *parent) :
	ContactAccountDataWidget(new JabberContactAccountDataManager(contactAccountData, parent), 
		contactAccountData, parent),
	Data(contactAccountData)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

JabberContactAccountDataWidget::~JabberContactAccountDataWidget()
{
}

void JabberContactAccountDataWidget::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/jabber_contact.ui"));
	//QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widgetById("password"));
	//passwordLineEdit->setEchoMode(QLineEdit::Password);
}
