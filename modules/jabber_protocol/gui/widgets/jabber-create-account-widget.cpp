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
#include "gui/windows/message-box.h"
#include "server/jabber-server-register-account.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(QWidget *parent) :
		AccountCreateWidget(parent), ShowConnectionOptions(false)
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

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	HaveJidIdentity = new ChooseIdentityWidget(this);
	connect(HaveJidIdentity, SIGNAL(identityChanged()), this, SLOT(iHaveAccountDataChanged()));
	gridLayout->addWidget(HaveJidIdentity, row++, 2, 1, 2);

	HaveJidRememberPassword = new QCheckBox(tr("Remember password"), this);
	HaveJidRememberPassword->setChecked(true);
	gridLayout->addWidget(HaveJidRememberPassword, row++, 2, 1, 2);

	AddThisAccount = new QPushButton(tr("Add this account"), this);
	AddThisAccount->setEnabled(false);
	connect(AddThisAccount, SIGNAL(clicked(bool)), this, SLOT(addThisAccount()));
	gridLayout->addWidget(AddThisAccount, row++, 1, 1, 4);

	HaveJidWidgets.append(numberLabel);
	HaveJidWidgets.append(AccountId);
	HaveJidWidgets.append(passwordLabel);
	HaveJidWidgets.append(AccountPassword);
	HaveJidWidgets.append(RemindPassword);
	HaveJidWidgets.append(HaveJidRememberPassword);
	HaveJidWidgets.append(descriptionLabel);
	HaveJidWidgets.append(HaveJidIdentity);
	HaveJidWidgets.append(AddThisAccount);
}

void JabberCreateAccountWidget::createRegisterAccountGui(QGridLayout *gridLayout, int &row)
{
	// Initialize settings
	ssl_ = 0;
	legacy_ssl_probe_ = true;
	port_ = 5222;

	QRadioButton *dontHaveJid = new QRadioButton(tr("I don't have a Jabber ID"), this);
	gridLayout->addWidget(dontHaveJid, row++, 0, 1, 4);

	QLabel *serverLabel = new QLabel(tr("Jabber server") + ":", this);
	gridLayout->addWidget(serverLabel, row, 1, Qt::AlignRight);
	Server = new QLineEdit(this);
	connect(Server, SIGNAL(textChanged(QString)), this, SLOT(registerAccountDataChanged()));
	gridLayout->addWidget(Server, row++, 2, 1, 2);

	QLabel *usernameLabel = new QLabel(tr("User name") + ":", this);
	gridLayout->addWidget(usernameLabel, row, 1, Qt::AlignRight);
	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textChanged(QString)), this, SLOT(registerAccountDataChanged()));
	gridLayout->addWidget(Username, row++, 2, 1, 2);

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

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	DontHaveJidIdentity = new ChooseIdentityWidget(this);
	connect(DontHaveJidIdentity, SIGNAL(identityChanged()), this, SLOT(registerAccountDataChanged()));
	gridLayout->addWidget(DontHaveJidIdentity, row++, 2, 1, 2);

	DontHaveJidRememberPassword = new QCheckBox(tr("Remember password"), this);
	DontHaveJidRememberPassword->setChecked(true);
	gridLayout->addWidget(DontHaveJidRememberPassword, row++, 2, 1, 2);

	RegisterAccount = new QPushButton(tr("Register"), this);
	RegisterAccount->setEnabled(false);
	connect(RegisterAccount, SIGNAL(clicked(bool)), this, SLOT(registerNewAccount()));
	gridLayout->addWidget(RegisterAccount, row++, 1, 1, 3);

	QLabel *moreOptionsLabel = new QLabel;
	moreOptionsLabel->setText(tr("More options:"));

	ExpandConnectionOptionsButton = new QPushButton(">");
	ExpandConnectionOptionsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(ExpandConnectionOptionsButton, SIGNAL(clicked()), this, SLOT(connectionOptionsChanged()));

	QWidget *moreOptions = new QWidget;
	QHBoxLayout *moreOptionsLayout = new QHBoxLayout;
	moreOptionsLayout->addWidget(moreOptionsLabel);
	moreOptionsLayout->addWidget(ExpandConnectionOptionsButton);
	moreOptionsLayout->setAlignment(ExpandConnectionOptionsButton, Qt::AlignLeft);
	moreOptionsLayout->insertStretch(-1);
	moreOptions->setLayout(moreOptionsLayout);

	gridLayout->addWidget(moreOptions, row++, 1, 1, 3);

        ConnectionOptions = new QGroupBox(this);
	ConnectionOptions->setTitle(tr("Connection settings"));
	ConnectionOptions->setVisible(false);

        QVBoxLayout *vboxLayout2 = new QVBoxLayout(ConnectionOptions);
        vboxLayout2->setSpacing(6);
        vboxLayout2->setMargin(9);

        CustomHostPort = new QCheckBox(ConnectionOptions);
        CustomHostPort->setText(tr("Manually Specify Server Host/Port")+":");
        vboxLayout2->addWidget(CustomHostPort);

        HostPortLayout = new QHBoxLayout();
        HostPortLayout->setSpacing(6);
        HostPortLayout->setMargin(0);

        CustomHostLabel = new QLabel(ConnectionOptions);
        CustomHostLabel->setText(tr("Host")+":");
        HostPortLayout->addWidget(CustomHostLabel);

        CustomHost = new QLineEdit(ConnectionOptions);
        HostPortLayout->addWidget(CustomHost);

        CustomPortLabel = new QLabel(ConnectionOptions);
        CustomPortLabel->setText(tr("Port")+":");
        HostPortLayout->addWidget(CustomPortLabel);

        CustomPort = new QLineEdit(ConnectionOptions);
        CustomPort->setMinimumSize(QSize(56, 0));
        CustomPort->setMaximumSize(QSize(56, 32767));
	CustomPort->setText(QString::number(port_));
        HostPortLayout->addWidget(CustomPort);

	// Manual Host/Port
	CustomHost->setEnabled(false);
	CustomHostLabel->setEnabled(false);
	CustomPort->setEnabled(false);
	CustomPortLabel->setEnabled(false);
	connect(CustomHostPort, SIGNAL(toggled(bool)), SLOT(hostToggled(bool)));

        vboxLayout2->addLayout(HostPortLayout);

        QHBoxLayout *EncryptionLayout = new QHBoxLayout();
        EncryptionLayout->setSpacing(6);
        EncryptionLayout->setMargin(0);
        EncryptionModeLabel = new QLabel(ConnectionOptions);
        EncryptionModeLabel->setText(tr("Encrypt connection")+":");
        EncryptionLayout->addWidget(EncryptionModeLabel);

        EncryptionMode = new QComboBox(ConnectionOptions);
	EncryptionMode->addItem(tr("Always"), 0);
	EncryptionMode->addItem(tr("When available"), 1);
	EncryptionMode->addItem(tr("Legacy SSL"), 2);
	EncryptionMode->setCurrentIndex(1);
	connect(EncryptionMode, SIGNAL(activated(int)), SLOT(sslActivated(int)));
        EncryptionLayout->addWidget(EncryptionMode);

        QSpacerItem *spacerItem = new QSpacerItem(151, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        EncryptionLayout->addItem(spacerItem);
        vboxLayout2->addLayout(EncryptionLayout);

        LegacySSLProbe = new QCheckBox(ConnectionOptions);
        LegacySSLProbe->setText(tr("Probe legacy SSL port"));
	LegacySSLProbe->setChecked(legacy_ssl_probe_);
        vboxLayout2->addWidget(LegacySSLProbe);

	gridLayout->addWidget(ConnectionOptions, row++, 1, 1, 4);

	DontHaveJidWidgets.append(serverLabel);
	DontHaveJidWidgets.append(Server);
	DontHaveJidWidgets.append(usernameLabel);
	DontHaveJidWidgets.append(Username);
	DontHaveJidWidgets.append(newPasswordLabel);
	DontHaveJidWidgets.append(NewPassword);
	DontHaveJidWidgets.append(reNewPasswordLabel);
	DontHaveJidWidgets.append(ReNewPassword);
	DontHaveJidWidgets.append(descriptionLabel);
	DontHaveJidWidgets.append(DontHaveJidIdentity);
	DontHaveJidWidgets.append(DontHaveJidRememberPassword);
	DontHaveJidWidgets.append(RegisterAccount);
	DontHaveJidWidgets.append(moreOptions);
}

bool JabberCreateAccountWidget::checkSSL()
{
	if(!QCA::isSupported("tls")) {
		MessageBox::msg(tr("Cannot enable SSL/TLS.  Plugin not found."));
		return false;
	}
	return true;
}

void JabberCreateAccountWidget::hostToggled(bool on)
{
	CustomHost->setEnabled(on);
	CustomPort->setEnabled(on);
	CustomHostLabel->setEnabled(on);
	CustomPortLabel->setEnabled(on);
	if (!on && EncryptionMode->currentIndex() == EncryptionMode->findData(2)) {
		EncryptionMode->setCurrentIndex(1);
	}
}

void JabberCreateAccountWidget::sslActivated(int i)
{
	if ((EncryptionMode->itemData(i) == 0 || EncryptionMode->itemData(i) == 2) && !checkSSL()) {
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(1));
	}
	else if (EncryptionMode->itemData(i) == 2 && !CustomHostPort->isChecked()) {
		MessageBox::msg(tr("Legacy SSL is only available in combination with manual host/port."));
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(1));
	}
}

void JabberCreateAccountWidget::connectionOptionsChanged()
{
	ShowConnectionOptions = !ShowConnectionOptions;
	ExpandConnectionOptionsButton->setText(ShowConnectionOptions ? "v" : ">");
	ConnectionOptions->setVisible(ShowConnectionOptions);
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
	AddThisAccount->setEnabled(!AccountId->text().isEmpty() && !AccountPassword->text().isEmpty()
				   && !HaveJidIdentity->identityName().isEmpty());
}

void JabberCreateAccountWidget::addThisAccount()
{
	Account jabberAccount;
	jabberAccount.setName(AccountName->text());
	jabberAccount.setId(AccountId->text());
	jabberAccount.setPassword(AccountPassword->text());
	jabberAccount.setRememberPassword(HaveJidRememberPassword->isChecked());

	emit accountCreated(jabberAccount);
}

void JabberCreateAccountWidget::registerAccountDataChanged()
{
	bool disable = Server->text().isEmpty() || Username->text().isEmpty() || NewPassword->text().isEmpty()
		       || ReNewPassword->text().isEmpty() || DontHaveJidIdentity->identityName().isEmpty();

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

	ssl_ = EncryptionMode->itemData(EncryptionMode->currentIndex()).toInt();
	legacy_ssl_probe_ = LegacySSLProbe->isChecked();
	opt_host_ = CustomHostPort->isChecked();
	host_ = CustomHost->text();
	port_ = CustomPort->text().toInt();

	JabberServerRegisterAccount *jsra = new JabberServerRegisterAccount(Server->text(), Username->text(), NewPassword->text(), legacy_ssl_probe_, ssl_ == 2, ssl_ == 0, opt_host_ ? host_ : QString(), port_);
	connect(jsra, SIGNAL(finished(JabberServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(JabberServerRegisterAccount *)));

	jsra->performAction();
}

void JabberCreateAccountWidget::registerNewAccountFinished(JabberServerRegisterAccount *jsra)
{
	if (jsra->result())
	{
		MessageBox::msg(tr("Registration was successful. Your new Jabber ID is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(jsra->jid()), false, "Information", this);

		Account jabberAccount;
		JabberAccountDetails *details = new JabberAccountDetails(jabberAccount.storage(), jabberAccount);

		jabberAccount.setDetails(details);
		jabberAccount.setName(AccountName->text());
		jabberAccount.setId(jsra->jid());
		jabberAccount.setPassword(NewPassword->text());
		details->setTlsOverrideDomain(jsra->client()->tlsOverrideDomain());
		details->setTlsOverrideCert(jsra->client()->tlsOverrideCert());
		jabberAccount.setRememberPassword(DontHaveJidRememberPassword->isChecked());

		emit accountCreated(jabberAccount);
	}
	else
		MessageBox::msg(tr("An error has occured while registration. Please try again later."), false, "Warning", this);

	delete jsra;
}
