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

#include "gadu-add-account-widget.h"

GaduAddAccountWidget::GaduAddAccountWidget(QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

GaduAddAccountWidget::~GaduAddAccountWidget()
{
}

void GaduAddAccountWidget::createGui()
{
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);

	gridLayout->setColumnMinimumWidth(0, 20);
	gridLayout->setColumnStretch(3, 10);

	int row = 0;

	QLabel *numberLabel = new QLabel(tr("Gadu-Gadu number") + ":", this);
	gridLayout->addWidget(numberLabel, row, 1, Qt::AlignRight);
	AccountId = new QLineEdit(this);
	AccountId->setValidator(new QIntValidator(1, 999999999, this));
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	gridLayout->addWidget(AccountId, row++, 2);
	
	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	gridLayout->addWidget(passwordLabel, row, 1);
	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(AccountPassword, row++, 2);
	
	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	gridLayout->addWidget(RememberPassword, row++, 2);
	
	RemindPassword = new QLabel(tr("Forgot Your Password?"), this);
	gridLayout->addWidget(RemindPassword, row++, 2);
	
	QLabel *descriptionLabel = new QLabel(tr("Account Identity") + ":", this);
	gridLayout->addWidget(descriptionLabel, row, 1);
	
	Identity = new ChooseIdentityWidget(this);
	connect(Identity, SIGNAL(identityChanged()), this, SLOT(dataChanged()));
	gridLayout->addWidget(Identity, row++, 2, 1, 2);
	
	QLabel *identityHelpLabel = new QLabel(tr("Select or enter the identity that will be associated with this account."), this);
	gridLayout->addWidget(identityHelpLabel, row, 2);

}

void GaduAddAccountWidget::dataChanged()
{
	RemindPassword->setEnabled(!AccountId->text().isEmpty());
	//AddThisAccount->setEnabled(!AccountId->text().isEmpty() && !AccountPassword->text().isEmpty()
	//			   && !Identity->identityName().isEmpty());
}

void GaduAddAccountWidget::addThisAccount()
{
	Account gaduAccount = Account::create();
	// TODO: 0.6.6 set protocol after details because of crash
	//gaduAccount.setProtocolName("gadu");
	GaduAccountDetails *gad = new GaduAccountDetails(gaduAccount);
	gad->setState(StorableObject::StateNew);
	gaduAccount.setDetails(gad);
	gaduAccount.setProtocolName("gadu");
	//gaduAccount.setName(AccountName->text());
	gaduAccount.setId(AccountId->text());
	gaduAccount.setPassword(AccountPassword->text());
	gaduAccount.setHasPassword(!AccountPassword->text().isEmpty());
	gaduAccount.setRememberPassword(RememberPassword->isChecked());

	emit accountCreated(gaduAccount);
}