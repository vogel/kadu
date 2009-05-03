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

#include "gui/widgets/configuration/configuration-widget.h"

#include "jabber-account.h"
#include "misc/misc.h"

#include "jabber-account-data-manager.h"
#include "jabber-configuration-dialog.h"

JabberConfigurationDialog::JabberConfigurationDialog(JabberAccount *accountData, QWidget *parent)
	: ConfigurationWindow("account", tr("Jabber account"), new JabberAccountDataManager(accountData)), AccountData(accountData)
{
	setAttribute(Qt::WA_DeleteOnClose);
	createGui();
}

JabberConfigurationDialog::~JabberConfigurationDialog()
{
}

void JabberConfigurationDialog::createGui()
{
	widget()->appendUiFile(dataPath("kadu/modules/configuration/jabber_protocol.ui"));

	QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("password"));
	passwordLineEdit->setEchoMode(QLineEdit::Password);
}
