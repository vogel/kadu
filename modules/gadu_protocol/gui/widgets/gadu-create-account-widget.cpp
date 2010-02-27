/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "accounts/account-manager.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocols-manager.h"
#include "url-handlers/url-handler-manager.h"
#include "icons-manager.h"

#include "../../server/gadu-server-register-account.h"
#include "gadu-account-details.h"
#include "gadu-protocol-factory.h"
#include "token-widget.h"

#include "gadu-create-account-widget.h"

GaduCreateAccountWidget::GaduCreateAccountWidget(QWidget *parent) :
		ModalConfigurationWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
	dataChanged();
}

GaduCreateAccountWidget::~GaduCreateAccountWidget()
{
}

void GaduCreateAccountWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Name") + ":", AccountName);

	NewPassword = new QLineEdit(this);
	connect(NewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	NewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ":", NewPassword);

	ReNewPassword = new QLineEdit(this);
	connect(ReNewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	ReNewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Retype Password") + ":", ReNewPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	layout->addWidget(RememberPassword);

	EMail = new QLineEdit(this);
	connect(EMail, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("E-Mail Address") + ":", EMail);

	IdentityCombo = new IdentitiesComboBox(this);
	connect(IdentityCombo, SIGNAL(activated(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ":", IdentityCombo);

	layout->addRow(0, new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this));

	MyTokenWidget = new TokenWidget(this);
	connect(MyTokenWidget, SIGNAL(modified()), this, SLOT(dataChanged()));
	layout->addRow(tr("Characters") + ":", MyTokenWidget);

	layout->addRow(0, new QLabel(tr("<font size='-1'><i>For verification purposes, please type the characters above.</i></font>"), this));

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	RegisterAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Regster Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	connect(RegisterAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	buttons->addButton(RegisterAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	dataChanged();
}

void GaduCreateAccountWidget::resetGui()
{
	AccountName->setText("");
	NewPassword->setText("");
	ReNewPassword->setText("");
	RememberPassword->setChecked(true);
	EMail->setText("");
	IdentityCombo->setCurrentIdentity(Identity::null);
	MyTokenWidget->setTokenValue("");
}

void GaduCreateAccountWidget::dataChanged()
{
	bool sameNameExists = AccountManager::instance()->byName(AccountName->text());

	bool disable = sameNameExists
			|| AccountName->text().isEmpty()
			|| NewPassword->text().isEmpty()
			|| ReNewPassword->text().isEmpty()
			|| EMail->text().indexOf(UrlHandlerManager::instance()->mailRegExp()) < 0
			|| MyTokenWidget->tokenValue().isEmpty()
			|| NewPassword->text() != ReNewPassword->text()
			|| !IdentityCombo->currentIdentity();

	RegisterAccountButton->setEnabled(!disable);

	if (AccountName->text().isEmpty()
			&& NewPassword->text().isEmpty()
			&& ReNewPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& EMail->text().isEmpty()
			&& IdentityCombo->currentIdentity().isNull()
			&& MyTokenWidget->tokenValue().isEmpty())
		setState(StateNotChanged);
	else
		setState(disable ? StateChangedDataInvalid : StateChangedDataValid);
}

void GaduCreateAccountWidget::apply()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::msg(tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"Password\" and \"Retype Password\") "
			"should be the same!"));
		return;
	}

	GaduServerRegisterAccount *gsra = new GaduServerRegisterAccount(EMail->text(), NewPassword->text(),
			MyTokenWidget->tokenId(), MyTokenWidget->tokenValue());
	connect(gsra, SIGNAL(finished(GaduServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(GaduServerRegisterAccount *)));

	gsra->performAction();
}

void GaduCreateAccountWidget::cancel()
{
	resetGui();
	emit cancelled();
}

void GaduCreateAccountWidget::registerNewAccountFinished(GaduServerRegisterAccount *gsra)
{
	if (gsra && gsra->result())
	{
		MessageDialog::msg(tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(gsra->uin()), false, "32x32/dialog-information.png", this);

		Account gaduAccount = Account::create();
		gaduAccount.setProtocolName("gadu");
		gaduAccount.setName(AccountName->text());
		gaduAccount.setId(QString::number(gsra->uin()));
		gaduAccount.setPassword(NewPassword->text());
		gaduAccount.setRememberPassword(RememberPassword->isChecked());

		resetGui(); // don't need that data anymore

		emit accountCreated(gaduAccount);
	}
	else
		MessageDialog::msg(tr("An error has occured while registration. Please try again later."), false, "32x32/dialog-warning.png", this);

	if (gsra)
		delete gsra;
}
