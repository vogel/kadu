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

#include "gui/windows/message-box.h"
#include "jabber-account.h"
#include "jabber-protocol-factory.h"

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(QWidget *parent) :
		AccountCreateWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

JabberCreateAccountWidget::~JabberCreateAccountWidget()
{
}

void JabberCreateAccountWidget::createGui()
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

	haveJidChanged(true);
}

void JabberCreateAccountWidget::createIHaveAccountGui(QGridLayout *gridLayout, int &row)
{
	QRadioButton *haveJid = new QRadioButton(tr("I have a Jabber ID"), this);
	haveJid->setChecked(true);
	connect(haveJid, SIGNAL(toggled(bool)), this, SLOT(haveJidChanged(bool)));
	gridLayout->addWidget(haveJid, row++, 0, 1, 4);

	QLabel *numberLabel = new QLabel(tr("Jabber ID") + ":", this);
	gridLayout->addWidget(numberLabel, row, 1, Qt::AlignRight);
	AccountId = new QLineEdit(this);
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

	QCheckBox *rememberPassword = new QCheckBox(tr("Remember password"), this);
	rememberPassword->setChecked(true);
	gridLayout->addWidget(rememberPassword, row++, 2, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	QComboBox *description = new QComboBox(this);
	gridLayout->addWidget(description, row++, 2, 1, 2);

	AddThisAccount = new QPushButton(tr("Add this account"), this);
	AddThisAccount->setEnabled(false);
	connect(AddThisAccount, SIGNAL(clicked(bool)), this, SLOT(addThisAccount()));
	gridLayout->addWidget(AddThisAccount, row++, 1, 1, 4);

	HaveJidWidgets.append(numberLabel);
	HaveJidWidgets.append(AccountId);
	HaveJidWidgets.append(passwordLabel);
	HaveJidWidgets.append(AccountPassword);
	HaveJidWidgets.append(RemindPassword);
	HaveJidWidgets.append(rememberPassword);
	HaveJidWidgets.append(descriptionLabel);
	HaveJidWidgets.append(description);
	HaveJidWidgets.append(AddThisAccount);
}

void JabberCreateAccountWidget::createRegisterAccountGui(QGridLayout *gridLayout, int &row)
{
	QRadioButton *dontHaveJid = new QRadioButton(tr("I don't have a Jabber ID"), this);
	gridLayout->addWidget(dontHaveJid, row++, 0, 1, 4);

	QLabel *newPasswordLabel = new QLabel(tr("New password") + ":", this);
	gridLayout->addWidget(newPasswordLabel, row, 1, Qt::AlignRight);
	NewPassword = new QLineEdit(this);
	connect(NewPassword, SIGNAL(textChanged(QString)), this, SLOT(registerAccountDataChanged()));
	NewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(NewPassword, row++, 2, 1, 2);

	QLabel *reNewPasswordLabel = new QLabel(tr("Retype password") + ":", this);
	gridLayout->addWidget(reNewPasswordLabel, row, 1, Qt::AlignRight);
	ReNewPassword = new QLineEdit(this);
	connect(ReNewPassword, SIGNAL(textChanged(QString)), this, SLOT(registerAccountDataChanged()));
	ReNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ReNewPassword, row++, 2, 1, 2);

	QLabel *eMailLabel = new QLabel(tr("Your e-mail address") + ":", this);
	gridLayout->addWidget(eMailLabel, row, 1, Qt::AlignRight);
	EMail = new QLineEdit(this);
	connect(EMail, SIGNAL(textChanged(QString)), this, SLOT(registerAccountDataChanged()));
	gridLayout->addWidget(EMail, row++, 2, 1, 2);

	registerAccount = new QPushButton(tr("Register"), this);
	registerAccount->setEnabled(false);
	connect(registerAccount, SIGNAL(clicked(bool)), this, SLOT(registerNewAccount()));
	gridLayout->addWidget(registerAccount, row++, 1, 1, 3);

	DontHaveJidWidgets.append(newPasswordLabel);
	DontHaveJidWidgets.append(NewPassword);
	DontHaveJidWidgets.append(reNewPasswordLabel);
	DontHaveJidWidgets.append(ReNewPassword);
	DontHaveJidWidgets.append(eMailLabel);
	DontHaveJidWidgets.append(EMail);
	DontHaveJidWidgets.append(registerAccount);
}

void JabberCreateAccountWidget::haveJidChanged(bool haveJid)
{
	foreach (QWidget *widget, HaveJidWidgets)
		widget->setVisible(haveJid);
	foreach (QWidget *widget, DontHaveJidWidgets)
		widget->setVisible(!haveJid);
}

void JabberCreateAccountWidget::iHaveAccountDataChanged()
{
	RemindPassword->setEnabled(!AccountId->text().isEmpty());
	AddThisAccount->setEnabled(!AccountId->text().isEmpty() && !AccountPassword->text().isEmpty());
}

void JabberCreateAccountWidget::addThisAccount()
{
	Account *jabberAccount = JabberProtocolFactory::instance()->newAccount();
	jabberAccount->setName(AccountName->text());
	jabberAccount->setId(AccountId->text());
	jabberAccount->setPassword(AccountPassword->text());

	emit accountCreated(jabberAccount);
}

void JabberCreateAccountWidget::registerAccountDataChanged()
{
	bool disable = NewPassword->text().isEmpty() || ReNewPassword->text().isEmpty();
	registerAccount->setEnabled(!disable);
}

void JabberCreateAccountWidget::registerNewAccount()
{
    	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageBox::msg(tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"New password\" and \"Retype password\") "
			"should be the same!"));
		return;
	}

	//JabberServerRegisterAccount *gsra = new JabberServerRegisterAccount(EMail->text(), NewPassword->text(),
	//								tokenWidget->tokenId(), tokenWidget->tokenValue());
	//connect(gsra, SIGNAL(finished(JabberServerRegisterAccount *)),
	//		this, SLOT(registerNewAccountFinished(JabberServerRegisterAccount *)));

	//gsra->performAction();
}

// void JabberCreateAccountWidget::registerNewAccountFinished(JabberServerRegisterAccount *gsra)
// {
// 	if (gsra->result())
// 	{
// 		MessageBox::msg(tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(gsra->uin()), false, "Information", this);
// 
// 		Account *jabberAccount = JabberProtocolFactory::instance()->newAccount();
// 		jabberAccount->setName(AccountName->text());
// 		jabberAccount->setId(QString::number(gsra->uin()));
// 		jabberAccount->setPassword(NewPassword->text());
// 
// 		emit accountCreated(jabberAccount);
// 	}
// 	else
// 		MessageBox::msg(tr("An error has occured while registration. Please try again later."), false, "Warning", this);
// 
// 	delete gsra;
// }
