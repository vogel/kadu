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

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(QWidget *parent) :
		AccountCreateWidget(parent), ShowConnectionOptions(false)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// Initialize settings
	ssl_ = 0;
	legacy_ssl_probe_ = true;
	port_ = 5222;
	
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
	gridLayout->setColumnStretch(2, 3);
	gridLayout->setColumnStretch(3, 0);
	gridLayout->setColumnStretch(4, 1);
	gridLayout->setColumnStretch(5, 1);

	int row = 0;

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
	
	QLabel *retypePasswordLabel = new QLabel(tr("Retype Password") + ":", this);
	gridLayout->addWidget(retypePasswordLabel, row, 1, Qt::AlignRight);
	RetypePassword = new QLineEdit(this);
	connect(RetypePassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	RetypePassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(RetypePassword, row++, 2, 1, 3);
	
	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	gridLayout->addWidget(RememberPassword, row++, 2, 1, 2);
	
	QLabel *descriptionLabel = new QLabel(tr("Account Identity") + ":", this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	Identity = new ChooseIdentityWidget(this);
	connect(Identity, SIGNAL(identityChanged()), this, SLOT(dataChanged()));
	gridLayout->addWidget(Identity, row++, 2, 1, 3);
	
	QLabel *identityHelpLabel = new QLabel(tr("Select or enter the identity that will be associated with this account."), this);
	gridLayout->addWidget(identityHelpLabel, row++, 2, 1, 3);
	
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
}

bool JabberCreateAccountWidget::checkSSL()
{
	if(!QCA::isSupported("tls")) {
		MessageDialog::msg(tr("Cannot enable SSL/TLS.  Plugin not found."));
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
		MessageDialog::msg(tr("Legacy SSL is only available in combination with manual host/port."));
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(1));
	}
}

void JabberCreateAccountWidget::connectionOptionsChanged()
{
	ShowConnectionOptions = !ShowConnectionOptions;
	ExpandConnectionOptionsButton->setText(ShowConnectionOptions ? "v" : ">");
	ConnectionOptions->setVisible(ShowConnectionOptions);
}

void JabberCreateAccountWidget::dataChanged()
{
	RemindPassword->setEnabled(!Username->text().isEmpty());
///	AddThisAccount->setEnabled(!Username->text().isEmpty() && !Password->text().isEmpty()
///				   && !Identity->identityName().isEmpty());
}

void JabberCreateAccountWidget::registerAccountDataChanged()
{
	bool disable = Domain->currentText().isEmpty() || Username->text().isEmpty() || Password->text().isEmpty()
		       || RetypePassword->text().isEmpty() || Identity->identityName().isEmpty();

///	RegisterAccount->setEnabled(!disable);
}

void JabberCreateAccountWidget::registerNewAccount()
{
    	if (Password->text() != RetypePassword->text())
	{
		MessageDialog::msg(tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"New password\" and \"Retype password\") "
			"should be the same!"));
		return;
	}

	ssl_ = EncryptionMode->itemData(EncryptionMode->currentIndex()).toInt();
	legacy_ssl_probe_ = LegacySSLProbe->isChecked();
	opt_host_ = CustomHostPort->isChecked();
	host_ = CustomHost->text();
	port_ = CustomPort->text().toInt();

	JabberServerRegisterAccount *jsra = new JabberServerRegisterAccount(Domain->currentText(), Username->text(), Password->text(), legacy_ssl_probe_, ssl_ == 2, ssl_ == 0, opt_host_ ? host_ : QString(), port_);
	connect(jsra, SIGNAL(finished(JabberServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(JabberServerRegisterAccount *)));

	jsra->performAction();
}

void JabberCreateAccountWidget::registerNewAccountFinished(JabberServerRegisterAccount *jsra)
{
	if (jsra->result())
	{
		MessageDialog::msg(tr("Registration was successful. Your new Jabber ID is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(jsra->jid()), false, "Information", this);

		Account jabberAccount;
		// TODO: 0.6.6 set protocol after details because of crash
		//jabberAccount.setProtocolName("jabber");
		JabberAccountDetails *details = new JabberAccountDetails(jabberAccount);
		details->setState(StorableObject::StateNew);
		jabberAccount.setDetails(details);
		jabberAccount.setProtocolName("jabber");
		//jabberAccount.setName(AccountName->text());
		jabberAccount.setId(jsra->jid());
		jabberAccount.setPassword(Password->text());
		details->setTlsOverrideDomain(jsra->client()->tlsOverrideDomain());
		details->setTlsOverrideCert(jsra->client()->tlsOverrideCert());
		jabberAccount.setRememberPassword(RememberPassword->isChecked());

		emit accountCreated(jabberAccount);
	}
	else
		MessageDialog::msg(tr("An error has occured while registration. Please try again later."), false, "Warning", this);

	delete jsra;
}
