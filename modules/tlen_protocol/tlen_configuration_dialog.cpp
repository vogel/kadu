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

#include "tlen_account.h"
#include "misc/misc.h"

#include "tlen-account-data-manager.h"
#include "tlen_configuration_dialog.h"

TlenConfigurationDialog::TlenConfigurationDialog(TlenAccount *accountData, QWidget *parent)
	: ConfigurationWindow("account", tr("Tlen account"), new TlenAccountDataManager(accountData)), AccountData(accountData)
{
	setAttribute(Qt::WA_DeleteOnClose);
	createGui();
}

TlenConfigurationDialog::~TlenConfigurationDialog()
{
}

void TlenConfigurationDialog::createGui()
{
	widget()->appendUiFile(dataPath("kadu/modules/configuration/tlen_protocol.ui"));
	QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("password"));
	passwordLineEdit->setEchoMode(QLineEdit::Password);
}
