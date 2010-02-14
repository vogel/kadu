
/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "gui/windows/message-dialog.h"
#include "icons-manager.h"
#include "protocols/protocols-manager.h"
#include "server/jabber-server-register-account.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(QWidget *parent) :
		QWidget(parent), ShowConnectionOptions(false)
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
  	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Name") + ":", AccountName);
	
	QWidget *jidWidget = new QWidget(this);
	QHBoxLayout *jidLayout = new QHBoxLayout(jidWidget);
	jidLayout->setSpacing(0);
	jidLayout->setMargin(0);

	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Username, 20);
	
	QLabel *atLabel = new QLabel("@", this);
	jidLayout->addWidget(atLabel);
	
	Domain = new QComboBox();
	Domain->setEditable(true);
	jidLayout->addWidget(Domain, 10);
	jidLayout->addStretch(100);
	
	layout->addRow(tr("Username") + ":", jidWidget);

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

	IdentityCombo = new IdentitiesComboBox(this);
	connect(IdentityCombo, SIGNAL(activated(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ":", IdentityCombo);

	layout->addRow(0, new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this));

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

	layout->addRow(0, moreOptions);

	QWidget *OptionsWidget = new QWidget(this);
        ConnectionOptions = new QGroupBox(OptionsWidget);
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

	layout->addRow(0, OptionsWidget);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	RegisterAccountButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-apply.png"), tr("Regster Account"), this);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-cancel.png"), tr("Cancel"), this);

	connect(RegisterAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	buttons->addButton(RegisterAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
	
	dataChanged();
}

bool JabberCreateAccountWidget::checkSSL()
{
	if (!QCA::isSupported("tls"))
	{
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
	if (!on && EncryptionMode->currentIndex() == EncryptionMode->findData(2))
		EncryptionMode->setCurrentIndex(1);
}

void JabberCreateAccountWidget::sslActivated(int i)
{
	if ((EncryptionMode->itemData(i) == 0 || EncryptionMode->itemData(i) == 2) && !checkSSL())
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(1));
	else if (EncryptionMode->itemData(i) == 2 && !CustomHostPort->isChecked())
	{
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
	bool disable = Domain->currentText().isEmpty() || Username->text().isEmpty() || NewPassword->text().isEmpty()
		       || ReNewPassword->text().isEmpty() || IdentityCombo->currentIndex() == -1;

	RegisterAccountButton->setEnabled(!disable);
}

void JabberCreateAccountWidget::apply()
{
    	if (NewPassword->text() != ReNewPassword->text())
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

	JabberServerRegisterAccount *jsra = new JabberServerRegisterAccount(Domain->currentText(), Username->text(), NewPassword->text(), legacy_ssl_probe_, ssl_ == 2, ssl_ == 0, opt_host_ ? host_ : QString(), port_);
	connect(jsra, SIGNAL(finished(JabberServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(JabberServerRegisterAccount *)));

	jsra->performAction();
}

void JabberCreateAccountWidget::cancel()
{
	resetGui();
	emit cancelled();
}

void JabberCreateAccountWidget::resetGui()
{
	AccountName->setText("");
	Username->setText("");
	Domain->setCurrentIndex(-1);
	NewPassword->setText("");
	ReNewPassword->setText("");
	RememberPassword->setChecked(true);
	IdentityCombo->setCurrentIdentity(Identity::null);
}

void JabberCreateAccountWidget::registerNewAccountFinished(JabberServerRegisterAccount *jsra)
{
	if (jsra->result())
	{
		MessageDialog::msg(tr("Registration was successful. Your new Jabber ID is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(jsra->jid()), false, "32x32/dialog-information.png", this);

		Account jabberAccount = Account::create();
		// TODO: 0.6.6 set protocol after details because of crash
		//jabberAccount.setProtocolName("jabber");
		JabberAccountDetails *details = new JabberAccountDetails(jabberAccount);
		details->setState(StorableObject::StateNew);
		jabberAccount.setDetails(details);
		jabberAccount.setProtocolName("jabber");
		//jabberAccount.setName(AccountName->text());
		jabberAccount.setId(jsra->jid());
		jabberAccount.setPassword(NewPassword->text());
		details->setTlsOverrideDomain(jsra->client()->tlsOverrideDomain());
		jabberAccount.setRememberPassword(RememberPassword->isChecked());

		emit accountCreated(jabberAccount);
	}
	else
		MessageDialog::msg(tr("An error has occured while registration. Please try again later."), false, "32x32/dialog-warning.png", this);

	delete jsra;
}
