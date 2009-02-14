/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
*/
#include <QtGui/QLineEdit>

//#include "icons_manager.h"

#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "misc.h"

#include "contact-account-data-widget.h"

ContactAccountDataWidget::ContactAccountDataWidget(ContactAccountDataManager *dataManager, ContactAccountData *contactAccountData, QWidget *parent)
	: ConfigurationWidget(dataManager, parent), Data(contactAccountData)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactAccountDataWidget::~ContactAccountDataWidget()
{
}

void ContactAccountDataWidget::createGui()
{
	appendUiFile(dataPath("kadu/configuration/contact-account-data.ui"));

	QLineEdit *idLineEdit = dynamic_cast<QLineEdit *>(widgetById("Id"));
	idLineEdit->setReadOnly(true);
	idLineEdit->setBackgroundMode(Qt::PaletteButton);

	QLineEdit *addrLineEdit = dynamic_cast<QLineEdit *>(widgetById("Addr"));
	addrLineEdit->setReadOnly(true);
	addrLineEdit->setBackgroundMode(Qt::PaletteButton);

	QLineEdit *versionLineEdit = dynamic_cast<QLineEdit *>(widgetById("Version"));
	versionLineEdit->setReadOnly(true);
	versionLineEdit->setBackgroundMode(Qt::PaletteButton);

	QLineEdit *dnsLineEdit = dynamic_cast<QLineEdit *>(widgetById("DnsName"));
	dnsLineEdit->setReadOnly(true);
	dnsLineEdit->setBackgroundMode(Qt::PaletteButton);

	QLineEdit *statusLineEdit = dynamic_cast<QLineEdit *>(widgetById("Status"));
	statusLineEdit->setReadOnly(true);
	statusLineEdit->setBackgroundMode(Qt::PaletteButton);
}
