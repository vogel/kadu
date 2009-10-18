/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLineEdit>

#include "configuration/configuration-contact-account-data-manager.h"
#include "contacts/account-data/contact-account-data.h"
#include "misc/misc.h"

#include "contact-account-data-widget.h"

ContactAccountDataWidget::ContactAccountDataWidget(ConfigurationContactAccountDataManager *dataManager, ContactAccountData *contactAccountData, QWidget *parent)
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

	QLineEdit *addrLineEdit = dynamic_cast<QLineEdit *>(widgetById("Addr"));
	addrLineEdit->setReadOnly(true);
	addrLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *versionLineEdit = dynamic_cast<QLineEdit *>(widgetById("Version"));
	versionLineEdit->setReadOnly(true);
	versionLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *dnsLineEdit = dynamic_cast<QLineEdit *>(widgetById("DnsName"));
	dnsLineEdit->setReadOnly(true);
	dnsLineEdit->setBackgroundRole(QPalette::Button);

	QLineEdit *statusLineEdit = dynamic_cast<QLineEdit *>(widgetById("Status"));
	statusLineEdit->setReadOnly(true);
	statusLineEdit->setBackgroundRole(QPalette::Button);
}
