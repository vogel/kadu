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

#include "gadu_account_data.h"
#include "icons_manager.h"

#include "gadu-account-data-manager.h"
#include "gadu_configuration_dialog.h"

GaduConfigurationDialog::GaduConfigurationDialog(GaduAccountData *accountData, QWidget *parent)
	: ConfigurationWindow("account", tr("Gadu-Gadu account"), new GaduAccountDataManager(accountData)), AccountData(accountData)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

GaduConfigurationDialog::~GaduConfigurationDialog()
{
}

void GaduConfigurationDialog::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/gadu_protocol.ui"));
	QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widgetById("password"));
	passwordLineEdit->setEchoMode(QLineEdit::Password);
}
