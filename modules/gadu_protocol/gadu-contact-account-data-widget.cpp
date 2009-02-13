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

#include "gadu-contact-account-data.h"
#include "gadu-contact-account-data-manager.h"
#include "misc.h"

#include "gadu-contact-account-data-widget.h"

GaduContactAccountDataWidget::GaduContactAccountDataWidget(GaduContactAccountData *contactAccountData, QWidget *parent)
	// TODO: remove new GaduAccountDataManager(accountData)) after use
	: ContactAccountDataWidget(new GaduContactAccountDataManager(contactAccountData), contactAccountData, parent), 
	    Data(contactAccountData)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

GaduContactAccountDataWidget::~GaduContactAccountDataWidget()
{
}

void GaduContactAccountDataWidget::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/gadu_contact.ui"));
	//QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widgetById("password"));
	//passwordLineEdit->setEchoMode(QLineEdit::Password);
}
