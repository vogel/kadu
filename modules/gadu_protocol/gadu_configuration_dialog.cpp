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

#include "html_document.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"

#include "server/gadu-server-register-account.h"

#include "gadu-account.h"
#include "gadu-account-data-manager.h"

#include "gadu_configuration_dialog.h"

GaduConfigurationDialog::GaduConfigurationDialog(GaduAccount *accountData, QWidget *parent)
	  // TODO: remove new GaduAccountDataManager(accountData)) after use
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
	widget()->appendUiFile(dataPath("kadu/modules/configuration/gadu_protocol.ui"));

	QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("gaduPassword"));
	passwordLineEdit->setEchoMode(QLineEdit::Password);

	QLineEdit *newPasswordLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("gaduNewPassword"));
	newPasswordLineEdit->setEchoMode(QLineEdit::Password);

	QPushButton *registerNewAccountButton = dynamic_cast<QPushButton *>(widget()->widgetById("gaduRegisterNewAccount"));
	connect(registerNewAccountButton, SIGNAL(clicked()), this, SLOT(registerNewAccountClicked()));
}

void GaduConfigurationDialog::registerNewAccountClicked()
{
	QLineEdit *newPasswordLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("gaduNewPassword"));
	QLineEdit *emailLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("gaduEmail"));

	QString password = newPasswordLineEdit->text();
	QString mail = emailLineEdit->text();

	if (password.isEmpty() || mail.isEmpty())
	{
		MessageBox::msg(tr("Please fill out all fields"), false, "Warning", this);
		return;
	}
/* TODO: 0.6.6
	if (mail.find(HtmlDocument::mailRegExp()) == -1)
	{
		MessageBox::msg(tr("Email address you have entered is not valid"), false, "Warning", this);
		return;
	}
*/
	setEnabled(false);

	GaduServerRegisterAccount *gsra = new GaduServerRegisterAccount(kadu, mail, password);
	connect(gsra, SIGNAL(finished(GaduServerConnector *)),
			this, SLOT(registerNewAccountFinished(GaduServerConnector *)));

	gsra->perform();
}

void GaduConfigurationDialog::registerNewAccountFinished(GaduServerConnector *gsc)
{
	GaduServerRegisterAccount *gsra = dynamic_cast<GaduServerRegisterAccount *>(gsc);
	if (!gsra)
		return;

	if (gsra->result())
	{
		QLineEdit *idLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("gaduId"));
		idLineEdit->setText(QString::number(gsra->uin()));

		QLineEdit *passwordLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("gaduPassword"));
		QLineEdit *newPasswordLineEdit = dynamic_cast<QLineEdit *>(widget()->widgetById("gaduNewPassword"));
		passwordLineEdit->setText(newPasswordLineEdit->text());

		MessageBox::msg(tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(gsra->uin()), false, "Information", this);
	}
	else
		MessageBox::msg(tr("An error has occured while registration. Please try again later."), false, "Warning", this);

	delete gsra;
	setEnabled(true);
}
