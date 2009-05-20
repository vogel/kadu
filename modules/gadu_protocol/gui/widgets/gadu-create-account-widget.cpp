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
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "gadu-account.h"
#include "gadu-protocol-factory.h"

#include "gadu-create-account-widget.h"

#include "token-widget.h"

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
	gridLayout->addWidget(AccountId, row++, 2, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	gridLayout->addWidget(passwordLabel, row, 1, Qt::AlignRight);
	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(AccountPassword, row, 2);
	QPushButton *remindPassword = new QPushButton(tr("Forgot password"), this);
	gridLayout->addWidget(remindPassword, row++, 3, Qt::AlignLeft);

	QCheckBox *rememberPassword = new QCheckBox(tr("Remember password"), this);
	rememberPassword->setChecked(true);
	gridLayout->addWidget(rememberPassword, row++, 2, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	QComboBox *description = new QComboBox(this);
	gridLayout->addWidget(description, row++, 2, 1, 2);

	QPushButton *addThisAccount = new QPushButton(tr("Add this account"), this);
	connect(addThisAccount, SIGNAL(clicked(bool)), this, SLOT(addThisAccount()));
	gridLayout->addWidget(addThisAccount, row++, 1, 1, 4);

	HaveNumberWidgets.append(numberLabel);
	HaveNumberWidgets.append(AccountId);
	HaveNumberWidgets.append(passwordLabel);
	HaveNumberWidgets.append(AccountPassword);
	HaveNumberWidgets.append(remindPassword);
	HaveNumberWidgets.append(rememberPassword);
	HaveNumberWidgets.append(descriptionLabel);
	HaveNumberWidgets.append(description);
	HaveNumberWidgets.append(addThisAccount);
}

void GaduCreateAccountWidget::createRegisterAccountGui(QGridLayout *gridLayout, int &row)
{
	QRadioButton *dontHaveNumber = new QRadioButton(tr("I don't have a Gadu-Gadu number"), this);
	gridLayout->addWidget(dontHaveNumber, row++, 0, 1, 4);

	QLabel *newPasswordLabel = new QLabel(tr("New password") + ":", this);
	gridLayout->addWidget(newPasswordLabel, row, 1, Qt::AlignRight);
	QLineEdit *newPassword = new QLineEdit(this);
	newPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(newPassword, row++, 2, 1, 2);

	QLabel *reNewPasswordLabel = new QLabel(tr("Retype password") + ":", this);
	gridLayout->addWidget(reNewPasswordLabel, row, 1, Qt::AlignRight);
	QLineEdit *reNewPassword = new QLineEdit(this);
	reNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(reNewPassword, row++, 2, 1, 2);

	QLabel *eMailLabel = new QLabel(tr("Your e-mail address") + ":", this);
	gridLayout->addWidget(eMailLabel, row, 1, Qt::AlignRight);
	QLineEdit *eMail = new QLineEdit(this);
	gridLayout->addWidget(eMail, row++, 2, 1, 2);

	QLabel *tokenLabel = new QLabel(tr("Type this code") + ":", this);
	gridLayout->addWidget(tokenLabel, row, 1, Qt::AlignRight);

	TokenWidget *tokenWidget = new TokenWidget(this);
	gridLayout->addWidget(tokenWidget, row++, 2, 1, 2);

	QPushButton *registerAccount = new QPushButton(tr("Register"), this);
	gridLayout->addWidget(registerAccount, row++, 1, 1, 3);

	QPushButton *addThisAccount = new QPushButton(tr("Add this account"), this);
	gridLayout->addWidget(addThisAccount, row++, 1, 1, 3);

	DontHaveNumberWidgets.append(newPasswordLabel);
	DontHaveNumberWidgets.append(newPassword);
	DontHaveNumberWidgets.append(reNewPasswordLabel);
	DontHaveNumberWidgets.append(reNewPassword);
	DontHaveNumberWidgets.append(eMailLabel);
	DontHaveNumberWidgets.append(eMail);
	DontHaveNumberWidgets.append(registerAccount);
	DontHaveNumberWidgets.append(tokenLabel);
	DontHaveNumberWidgets.append(tokenWidget);
	DontHaveNumberWidgets.append(addThisAccount);
}

void GaduCreateAccountWidget::haveNumberChanged(bool haveNumber)
{
	foreach (QWidget *widget, HaveNumberWidgets)
		widget->setVisible(haveNumber);
	foreach (QWidget *widget, DontHaveNumberWidgets)
		widget->setVisible(!haveNumber);
}

void GaduCreateAccountWidget::addThisAccount()
{
	Account *gaduAccount = GaduProtocolFactory::instance()->newAccount();
	gaduAccount->setName(AccountName->text());
	gaduAccount->setId(AccountId->text());
	gaduAccount->setPassword(AccountPassword->text());

	emit accountCreated(gaduAccount);
}
