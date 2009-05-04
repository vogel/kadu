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

#include "tlen-contact-account-data.h"
#include "tlen-contact-account-data-manager.h"
#include "misc/misc.h"

#include "tlen-contact-account-data-widget.h"

TlenContactAccountDataWidget::TlenContactAccountDataWidget(TlenContactAccountData *contactAccountData, QWidget *parent) :
	ContactAccountDataWidget(new TlenContactAccountDataManager(contactAccountData, parent), 
		contactAccountData, parent),
	Data(contactAccountData)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

TlenContactAccountDataWidget::~TlenContactAccountDataWidget()
{
}

void TlenContactAccountDataWidget::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/tlen_contact.ui"));
	//QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widgetById("password"));
	//passwordLineEdit->setEchoMode(QLineEdit::Password);
}
