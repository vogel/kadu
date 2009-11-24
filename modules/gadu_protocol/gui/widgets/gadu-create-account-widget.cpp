 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "gui/widgets/choose-identity-widget.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocols-manager.h"
#include "../../server/gadu-server-register-account.h"
#include "html_document.h"
#include "gadu-account-details.h"
#include "gadu-protocol-factory.h"
#include "token-widget.h"

#include "gadu-create-account-widget.h"

GaduCreateAccountWidget::GaduCreateAccountWidget(QWidget *parent) :
		AccountCreateWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

GaduCreateAccountWidget::~GaduCreateAccountWidget()
{
}

void GaduCreateAccountWidget::createGui()
{
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);

	gridLayout->setColumnMinimumWidth(0, 20);
	gridLayout->setColumnStretch(3, 10);

	int row = 0;

	QLabel *nameLabel = new QLabel(tr("Account name") + ":", this);
	gridLayout->addWidget(nameLabel, row, 1, Qt::AlignRight);
	AccountName = new QLineEdit(this);
	gridLayout->addWidget(AccountName, row++, 2, 1, 2);

	createIHaveAccountGui(gridLayout, row);
	createRegisterAccountGui(gridLayout, row);

	haveNumberChanged(true);
}

void GaduCreateAccountWidget::createIHaveAccountGui(QGridLayout *gridLayout, int &row)
{
	QRadioButton *haveNumber = new QRadioButton(tr("I have a Gadu-Gadu number"), this);
	haveNumber->setChecked(true);
	connect(haveNumber, SIGNAL(toggled(bool)), this, SLOT(haveNumberChanged(bool)));
	gridLayout->addWidget(haveNumber, row++, 0, 1, 4);

	QLabel *numberLabel = new QLabel(tr("Gadu-Gadu number") + ":", this);
	gridLayout->addWidget(numberLabel, row, 1, Qt::AlignRight);
	AccountId = new QLineEdit(this);
	AccountId->setValidator(new QIntValidator(1, 999999999, this));
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(iHaveAccountDataChanged()));
	gridLayout->addWidget(AccountId, row++, 2, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	gridLayout->addWidget(passwordLabel, row, 1, Qt::AlignRight);
	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(iHaveAccountDataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(AccountPassword, row, 2);
	RemindPassword = new QPushButton(tr("Forgot password"), this);
	RemindPassword->setEnabled(false);
	gridLayout->addWidget(RemindPassword, row++, 3, Qt::AlignLeft);

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	HaveNumberIdentity = new ChooseIdentityWidget(this);
	connect(HaveNumberIdentity, SIGNAL(identityChanged()), this, SLOT(iHaveAccountDataChanged()));
	gridLayout->addWidget(HaveNumberIdentity, row++, 2, 1, 2);

	HaveNumberRememberPassword = new QCheckBox(tr("Remember password"), this);
	HaveNumberRememberPassword->setChecked(true);
	gridLayout->addWidget(HaveNumberRememberPassword, row++, 2, 1, 2);

	AddThisAccount = new QPushButton(tr("Add this account"), this);
	AddThisAccount->setEnabled(false);
	connect(AddThisAccount, SIGNAL(clicked(bool)), this, SLOT(addThisAccount()));
	gridLayout->addWidget(AddThisAccount, row++, 1, 1, 4);

	HaveNumberWidgets.append(numberLabel);
	HaveNumberWidgets.append(AccountId);
	HaveNumberWidgets.append(passwordLabel);
	HaveNumberWidgets.append(AccountPassword);
	HaveNumberWidgets.append(RemindPassword);
	HaveNumberWidgets.append(descriptionLabel);
	HaveNumberWidgets.append(HaveNumberIdentity);
	HaveNumberWidgets.append(HaveNumberRememberPassword);
	HaveNumberWidgets.append(AddThisAccount);
}

void GaduCreateAccountWidget::createRegisterAccountGui(QGridLayout *gridLayout, int &row)
{
	QRadioButton *dontHaveNumber = new QRadioButton(tr("I don't have a Gadu-Gadu number"), this);
	gridLayout->addWidget(dontHaveNumber, row++, 0, 1, 4);

	QLabel *newPasswordLabel = new QLabel(tr("New password") + ":", this);
	gridLayout->addWidget(newPasswordLabel, row, 1, Qt::AlignRight);
	NewPassword = new QLineEdit(this);
	connect(NewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(registerAccountDataChanged()));
	NewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(NewPassword, row++, 2, 1, 2);

	QLabel *reNewPasswordLabel = new QLabel(tr("Retype password") + ":", this);
	gridLayout->addWidget(reNewPasswordLabel, row, 1, Qt::AlignRight);
	ReNewPassword = new QLineEdit(this);
	connect(ReNewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(registerAccountDataChanged()));
	ReNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ReNewPassword, row++, 2, 1, 2);

	QLabel *eMailLabel = new QLabel(tr("Your e-mail address") + ":", this);
	gridLayout->addWidget(eMailLabel, row, 1, Qt::AlignRight);
	EMail = new QLineEdit(this);
	connect(EMail, SIGNAL(textChanged(const QString &)), this, SLOT(registerAccountDataChanged()));
	gridLayout->addWidget(EMail, row++, 2, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	DontHaveNumberIdentity = new ChooseIdentityWidget(this);
	connect(DontHaveNumberIdentity, SIGNAL(identityChanged()), this, SLOT(registerAccountDataChanged()));
	gridLayout->addWidget(DontHaveNumberIdentity, row++, 2, 1, 2);

	QLabel *tokenLabel = new QLabel(tr("Type this code") + ":", this);
	gridLayout->addWidget(tokenLabel, row, 1, Qt::AlignRight);

	tokenWidget = new TokenWidget(this);
	connect(tokenWidget, SIGNAL(modified()), this, SLOT(registerAccountDataChanged()));
	gridLayout->addWidget(tokenWidget, row++, 2, 1, 2);

	DontHaveNumberRememberPassword = new QCheckBox(tr("Remember password"), this);
	DontHaveNumberRememberPassword->setChecked(true);
	gridLayout->addWidget(DontHaveNumberRememberPassword, row++, 2, 1, 2);

	registerAccount = new QPushButton(tr("Register"), this);
	registerAccount->setEnabled(false);
	connect(registerAccount, SIGNAL(clicked(bool)), this, SLOT(registerNewAccount()));
	gridLayout->addWidget(registerAccount, row++, 1, 1, 3);

	DontHaveNumberWidgets.append(newPasswordLabel);
	DontHaveNumberWidgets.append(NewPassword);
	DontHaveNumberWidgets.append(reNewPasswordLabel);
	DontHaveNumberWidgets.append(ReNewPassword);
	DontHaveNumberWidgets.append(eMailLabel);
	DontHaveNumberWidgets.append(EMail);
	DontHaveNumberWidgets.append(descriptionLabel);
	DontHaveNumberWidgets.append(DontHaveNumberIdentity);
	DontHaveNumberWidgets.append(tokenLabel);
	DontHaveNumberWidgets.append(tokenWidget);
	DontHaveNumberWidgets.append(DontHaveNumberRememberPassword);
	DontHaveNumberWidgets.append(registerAccount);
}

void GaduCreateAccountWidget::haveNumberChanged(bool haveNumber)
{
	foreach (QWidget *widget, HaveNumberWidgets)
		widget->setVisible(haveNumber);
	foreach (QWidget *widget, DontHaveNumberWidgets)
		widget->setVisible(!haveNumber);
}

void GaduCreateAccountWidget::iHaveAccountDataChanged()
{
	RemindPassword->setEnabled(!AccountId->text().isEmpty());
	AddThisAccount->setEnabled(!AccountId->text().isEmpty() && !AccountPassword->text().isEmpty()
				   && !HaveNumberIdentity->identityName().isEmpty());
}

void GaduCreateAccountWidget::addThisAccount()
{
	Account gaduAccount;
	gaduAccount.data()->setProtocolName("gadu");
	gaduAccount.data()->protocolRegistered(ProtocolsManager::instance()->byName("gadu"));
	gaduAccount.setDetails(new GaduAccountDetails(gaduAccount));
	gaduAccount.setName(AccountName->text());
	gaduAccount.setId(AccountId->text());
	gaduAccount.setPassword(AccountPassword->text());
	gaduAccount.setHasPassword(!AccountPassword->text().isEmpty());
	gaduAccount.setRememberPassword(HaveNumberRememberPassword->isChecked());

	emit accountCreated(gaduAccount);
}

void GaduCreateAccountWidget::registerAccountDataChanged()
{
	bool disable = NewPassword->text().isEmpty() || ReNewPassword->text().isEmpty()
		      || EMail->text().indexOf(HtmlDocument::mailRegExp()) < 0 || tokenWidget->tokenValue().isEmpty()
		      || DontHaveNumberIdentity->identityName().isEmpty();

	registerAccount->setEnabled(!disable);
}

void GaduCreateAccountWidget::registerNewAccount()
{
    if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::msg(tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"New password\" and \"Retype password\") "
			"should be the same!"));
		return;
	}

	GaduServerRegisterAccount *gsra = new GaduServerRegisterAccount(EMail->text(), NewPassword->text(),
			tokenWidget->tokenId(), tokenWidget->tokenValue());
	connect(gsra, SIGNAL(finished(GaduServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(GaduServerRegisterAccount *)));

	gsra->performAction();
}

void GaduCreateAccountWidget::registerNewAccountFinished(GaduServerRegisterAccount *gsra)
{
	if (gsra->result())
	{
		MessageDialog::msg(tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(gsra->uin()), false, "Information", this);
		
		Account gaduAccount;
		gaduAccount.data()->protocolRegistered(ProtocolsManager::instance()->byName("gadu"));
		gaduAccount.setDetails(new GaduAccountDetails(gaduAccount));
		gaduAccount.setName(AccountName->text());
		gaduAccount.setId(QString::number(gsra->uin()));
		gaduAccount.setPassword(NewPassword->text());
		gaduAccount.setRememberPassword(DontHaveNumberRememberPassword->isChecked());

		emit accountCreated(gaduAccount);
	}
	else
		MessageDialog::msg(tr("An error has occured while registration. Please try again later."), false, "Warning", this);

	delete gsra;
}
