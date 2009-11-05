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

#include "tlen-contact.h"
#include "tlen-contact-manager.h"
#include "misc/misc.h"

#include "tlen-contact-widget.h"

TlenContactWidget::TlenContactWidget(TlenContact *contact, QWidget *parent) :
	ContactWidget(new TlenContactManager(contact, parent), 
		contact, parent),
	Data(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

TlenContactWidget::~TlenContactWidget()
{
}

void TlenContactWidget::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/tlen_contact.ui"));
	//QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widgetById("password"));
	//passwordLineEdit->setEchoMode(QLineEdit::Password);
}
