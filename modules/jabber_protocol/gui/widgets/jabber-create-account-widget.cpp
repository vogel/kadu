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
#include "server/jabber-server-register-account.h"
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

	QLabel *serverLabel = new QLabel(tr("Jabber server") + ":", this);
	gridLayout->addWidget(serverLabel, row, 1, Qt::AlignRight);
	Server = new QLineEdit(this);
	connect(Server, SIGNAL(textChanged(QString)), this, SLOT(RegisterAccountDataChanged()));
	gridLayout->addWidget(Server, row++, 2, 1, 2);

	QLabel *usernameLabel = new QLabel(tr("User name") + ":", this);
	gridLayout->addWidget(usernameLabel, row, 1, Qt::AlignRight);
	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textChanged(QString)), this, SLOT(RegisterAccountDataChanged()));
	gridLayout->addWidget(Username, row++, 2, 1, 2);

	QLabel *newPasswordLabel = new QLabel(tr("New password") + ":", this);
	gridLayout->addWidget(newPasswordLabel, row, 1, Qt::AlignRight);
	NewPassword = new QLineEdit(this);
	connect(NewPassword, SIGNAL(textChanged(QString)), this, SLOT(RegisterAccountDataChanged()));
	NewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(NewPassword, row++, 2, 1, 2);

	QLabel *reNewPasswordLabel = new QLabel(tr("Retype password") + ":", this);
	gridLayout->addWidget(reNewPasswordLabel, row, 1, Qt::AlignRight);
	ReNewPassword = new QLineEdit(this);
	connect(ReNewPassword, SIGNAL(textChanged(QString)), this, SLOT(RegisterAccountDataChanged()));
	ReNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ReNewPassword, row++, 2, 1, 2);

	RegisterAccount = new QPushButton(tr("Register"), this);
	RegisterAccount->setEnabled(false);
	connect(RegisterAccount, SIGNAL(clicked(bool)), this, SLOT(registerNewAccount()));
	gridLayout->addWidget(RegisterAccount, row++, 1, 1, 3);

        ConnectionOptions = new QGroupBox(this);
	ConnectionOptions->setTitle(tr("Connection settings"));

        QVBoxLayout *vboxLayout2 = new QVBoxLayout(ConnectionOptions);
        vboxLayout2->setSpacing(6);
        vboxLayout2->setMargin(9);

        ck_host = new QCheckBox(ConnectionOptions);
        ck_host->setText(tr("Manually Specify Server Host/Port")+":");
        vboxLayout2->addWidget(ck_host);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setMargin(0);

        lb_host = new QLabel(ConnectionOptions);
        lb_host->setText(tr("Host")+":");
        hboxLayout->addWidget(lb_host);

        le_host = new QLineEdit(ConnectionOptions);
        hboxLayout->addWidget(le_host);

        lb_port = new QLabel(ConnectionOptions);
        lb_port->setText(tr("Port")+":");
        hboxLayout->addWidget(lb_port);

        le_port = new QLineEdit(ConnectionOptions);
        le_port->setMinimumSize(QSize(56, 0));
        le_port->setMaximumSize(QSize(56, 32767));
        hboxLayout->addWidget(le_port);

        vboxLayout2->addLayout(hboxLayout);

        QHBoxLayout *hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setMargin(0);
        lb_ssl = new QLabel(ConnectionOptions);
        lb_ssl->setText(tr("Encrypt connection")+":");
        hboxLayout1->addWidget(lb_ssl);

        cb_ssl = new QComboBox(ConnectionOptions);
	cb_ssl->addItem(tr("Always"), 0);
	cb_ssl->addItem(tr("When available"), 1);
	cb_ssl->addItem(tr("Legacy SSL"), 2);
	cb_ssl->setCurrentIndex(1);
        hboxLayout1->addWidget(cb_ssl);

        QSpacerItem *spacerItem = new QSpacerItem(151, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        hboxLayout1->addItem(spacerItem);
        vboxLayout2->addLayout(hboxLayout1);

        ck_legacy_ssl_probe = new QCheckBox(ConnectionOptions);
        ck_legacy_ssl_probe->setText(tr("Probe legacy SSL port"));
        vboxLayout2->addWidget(ck_legacy_ssl_probe);

	gridLayout->addWidget(ConnectionOptions, row++, 1, 1, 4);

	DontHaveJidWidgets.append(serverLabel);
	DontHaveJidWidgets.append(Server);
	DontHaveJidWidgets.append(usernameLabel);
	DontHaveJidWidgets.append(Username);
	DontHaveJidWidgets.append(newPasswordLabel);
	DontHaveJidWidgets.append(NewPassword);
	DontHaveJidWidgets.append(reNewPasswordLabel);
	DontHaveJidWidgets.append(ReNewPassword);
	DontHaveJidWidgets.append(RegisterAccount);
	DontHaveJidWidgets.append(ConnectionOptions);
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
	RegisterAccount->setEnabled(!disable);
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

	JabberServerRegisterAccount *jsra = new JabberServerRegisterAccount(Server->text(), Username->text(), NewPassword->text());
	connect(jsra, SIGNAL(finished(JabberServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(JabberServerRegisterAccount *)));

	jsra->performAction();
}

void JabberCreateAccountWidget::registerNewAccountFinished(JabberServerRegisterAccount *jsra)
{
	if (jsra->result())
	{
		MessageBox::msg(tr("Registration was successful. Your new Jabber ID is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(jsra->jid()), false, "Information", this);

		Account *jabberAccount = JabberProtocolFactory::instance()->newAccount();
		jabberAccount->setName(AccountName->text());
		jabberAccount->setId(jsra->jid());
		jabberAccount->setPassword(NewPassword->text());

		emit accountCreated(jabberAccount);
	}
	else
		MessageBox::msg(tr("An error has occured while registration. Please try again later."), false, "Warning", this);

	delete jsra;
}
