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
#include "server/jabber-server-register-account.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-add-account-widget.h"

JabberAddAccountWidget::JabberAddAccountWidget(QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

JabberAddAccountWidget::~JabberAddAccountWidget()
{
}

void JabberAddAccountWidget::createGui()
{
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);

	gridLayout->setColumnMinimumWidth(0, 20);
	gridLayout->setColumnStretch(2, 3);
	gridLayout->setColumnStretch(3, 0);
	gridLayout->setColumnStretch(4, 1);
	gridLayout->setColumnStretch(5, 1);

	int row = 0;
	
	QLabel *accountNameLabel = new QLabel(tr("Account Name") + ":", this);
	gridLayout->addWidget(accountNameLabel, row, 1);
	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	gridLayout->addWidget(AccountName, row++, 2);

	QLabel *numberLabel = new QLabel(tr("Username") + ":", this);
	gridLayout->addWidget(numberLabel, row, 1, Qt::AlignRight);
	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	gridLayout->addWidget(Username, row, 2, 1, 1);
	
	QLabel *atLabel = new QLabel("@", this);
	gridLayout->addWidget(atLabel, row, 3, 1, 1);
	
	Domain = new QComboBox();
	Domain->setEditable(true);
	gridLayout->addWidget(Domain, row++, 4, 1, 2);
	
	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	gridLayout->addWidget(passwordLabel, row, 1, Qt::AlignRight);
	Password = new QLineEdit(this);
	connect(Password, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	Password->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(Password, row++, 2, 1, 3);
	
	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	gridLayout->addWidget(RememberPassword, row++, 2, 1, 2);
	
	RemindPassword = new QLabel(tr("Forgot Your Password?"), this);
	gridLayout->addWidget(RemindPassword, row++, 2, Qt::AlignLeft);
	
	QLabel *descriptionLabel = new QLabel(tr("Account Identity") + ":", this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	Identity = new ChooseIdentityWidget(this);
	connect(Identity, SIGNAL(identityChanged()), this, SLOT(dataChanged()));
	gridLayout->addWidget(Identity, row++, 2, 1, 3);
	
	QLabel *identityHelpLabel = new QLabel(tr("Select or enter the identity that will be associated with this account."), this);
	gridLayout->addWidget(identityHelpLabel, row, 2, 1, 3);
}

void JabberAddAccountWidget::dataChanged()
{
	RemindPassword->setEnabled(!Username->text().isEmpty());
	///AddThisAccount->setEnabled(!Username->text().isEmpty() && !Password->text().isEmpty()
	///			   && !Identity->identityName().isEmpty());
}

void JabberAddAccountWidget::apply()
{
	Account jabberAccount = Account::create();
	// TODO: 0.6.6 set protocol after details because of crash
	//jabberAccount.setProtocolName("jabber");
	JabberAccountDetails *details = new JabberAccountDetails(jabberAccount);
	details->setState(StorableObject::StateNew);
	details->setResource("Kadu");
	details->setPriority(5);
	jabberAccount.setDetails(details);
	jabberAccount.setProtocolName("jabber");
///	jabberAccount.setName(AccountName->text());
	jabberAccount.setId(Username->text() + "@" + Domain->currentText());
	jabberAccount.setPassword(Password->text());
	jabberAccount.setHasPassword(!Password->text().isEmpty());
	jabberAccount.setRememberPassword(RememberPassword->isChecked());

	emit accountCreated(jabberAccount);
}