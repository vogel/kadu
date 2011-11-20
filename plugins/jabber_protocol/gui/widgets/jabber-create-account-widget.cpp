/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QGridLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "gui/windows/jabber-wait-for-account-register-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"
#include "server/jabber-server-register-account.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(bool showButtons, QWidget *parent) :
		AccountCreateWidget(parent), ShowConnectionOptions(false)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// Initialize settings
	ssl_ = 0;
	legacy_ssl_probe_ = true;
	port_ = 5222U;

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)), this, SLOT(dataChanged()));

	createGui(showButtons);
	resetGui();
}

JabberCreateAccountWidget::~JabberCreateAccountWidget()
{
}

void JabberCreateAccountWidget::createGui(bool showButtons)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	QWidget *jidWidget = new QWidget(this);
	QGridLayout *jidLayout = new QGridLayout(jidWidget);
	jidLayout->setSpacing(0);
	jidLayout->setMargin(0);
	jidLayout->setColumnStretch(0, 2);
	jidLayout->setColumnStretch(2, 2);

	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Username);

	QLabel *atLabel = new QLabel("@", this);
	jidLayout->addWidget(atLabel, 0, 1);

	Domain = new QComboBox();
	Domain->setEditable(true);
	connect(Domain, SIGNAL(currentIndexChanged(QString)), this, SLOT(dataChanged()));
	connect(Domain, SIGNAL(editTextChanged(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Domain, 0, 2);

	layout->addRow(tr("Username") + ':', jidWidget);

	NewPassword = new QLineEdit(this);
	connect(NewPassword, SIGNAL(textEdited(const QString &)), this, SLOT(dataChanged()));
	NewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', NewPassword);

	ReNewPassword = new QLineEdit(this);
	connect(ReNewPassword, SIGNAL(textEdited(const QString &)), this, SLOT(dataChanged()));
	ReNewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Retype Password") + ':', ReNewPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	layout->addWidget(RememberPassword);

	IdentityCombo = new IdentitiesComboBox(true, this);
	connect(IdentityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ':', IdentityCombo);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	QLabel *moreOptionsLabel = new QLabel;
	moreOptionsLabel->setText(tr("More options:"));

	ExpandConnectionOptionsButton = new QPushButton(">");
	ExpandConnectionOptionsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(ExpandConnectionOptionsButton, SIGNAL(clicked()), this, SLOT(connectionOptionsChanged()));

	QWidget *moreOptions = new QWidget;
	QHBoxLayout *moreOptionsLayout = new QHBoxLayout(moreOptions);
	moreOptionsLayout->addWidget(moreOptionsLabel);
	moreOptionsLayout->addWidget(ExpandConnectionOptionsButton);
	moreOptionsLayout->setAlignment(ExpandConnectionOptionsButton, Qt::AlignLeft);
	moreOptionsLayout->insertStretch(-1);
	moreOptions->setLayout(moreOptionsLayout);

	mainLayout->addWidget(moreOptions);

	OptionsWidget = new QWidget(this);
	QHBoxLayout *optionsLayout = new QHBoxLayout((OptionsWidget));
	QGroupBox *connectionOptions = new QGroupBox(OptionsWidget);
	optionsLayout->addWidget(connectionOptions);
	connectionOptions->setTitle(tr("Connection settings"));

	QVBoxLayout *vboxLayout2 = new QVBoxLayout(connectionOptions);
	vboxLayout2->setSpacing(6);
	vboxLayout2->setMargin(9);

	CustomHostPort = new QCheckBox(connectionOptions);
	CustomHostPort->setText(tr("Manually Specify Server Host/Port") + ':');
	vboxLayout2->addWidget(CustomHostPort);
	connect(CustomHostPort, SIGNAL(toggled(bool)), SLOT(hostToggled(bool)));

	HostPortLayout = new QHBoxLayout();
	HostPortLayout->setSpacing(6);
	HostPortLayout->setMargin(0);

	CustomHostLabel = new QLabel(connectionOptions);
	CustomHostLabel->setText(tr("Host") + ':');
	HostPortLayout->addWidget(CustomHostLabel);

	CustomHost = new QLineEdit(connectionOptions);
	HostPortLayout->addWidget(CustomHost);

	CustomPortLabel = new QLabel(connectionOptions);
	CustomPortLabel->setText(tr("Port") + ':');
	HostPortLayout->addWidget(CustomPortLabel);

	CustomPort = new QLineEdit(connectionOptions);
	CustomPort->setMinimumSize(QSize(56, 0));
	CustomPort->setMaximumSize(QSize(56, 32767));
	CustomPort->setText(QString::number(port_));
	HostPortLayout->addWidget(CustomPort);

	vboxLayout2->addLayout(HostPortLayout);

	QHBoxLayout *EncryptionLayout = new QHBoxLayout();
	EncryptionLayout->setSpacing(6);
	EncryptionLayout->setMargin(0);
	EncryptionModeLabel = new QLabel(connectionOptions);
	EncryptionModeLabel->setText(tr("Encrypt connection") + ':');
	EncryptionLayout->addWidget(EncryptionModeLabel);

	EncryptionMode = new QComboBox(connectionOptions);
	EncryptionMode->addItem(tr("Always"), 0);
	EncryptionMode->addItem(tr("When available"), 1);
	EncryptionMode->addItem(tr("Legacy SSL"), 2);
	connect(EncryptionMode, SIGNAL(activated(int)), SLOT(sslActivated(int)));
	EncryptionLayout->addWidget(EncryptionMode);

	QSpacerItem *spacerItem = new QSpacerItem(151, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	EncryptionLayout->addItem(spacerItem);
	vboxLayout2->addLayout(EncryptionLayout);

	LegacySSLProbe = new QCheckBox(connectionOptions);
	LegacySSLProbe->setText(tr("Probe legacy SSL port"));
	vboxLayout2->addWidget(LegacySSLProbe);

	mainLayout->addWidget(OptionsWidget);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	RegisterAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Register Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(RegisterAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(RegisterAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	if (!showButtons)
		buttons->hide();
}

bool JabberCreateAccountWidget::checkSSL()
{
	if (!QCA::isSupported("tls"))
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Cannot enable secure connection. SSL/TLS plugin not found."), QMessageBox::Ok, this);
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
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Legacy secure connection (SSL) is only available in combination with manual host/port."), QMessageBox::Ok, this);
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(1));
	}
}

void JabberCreateAccountWidget::connectionOptionsChanged()
{
	ShowConnectionOptions = !ShowConnectionOptions;
	ExpandConnectionOptionsButton->setText(ShowConnectionOptions ? "v" : ">");
	OptionsWidget->setVisible(ShowConnectionOptions);
}

void JabberCreateAccountWidget::dataChanged()
{
	bool valid = !Domain->currentText().isEmpty()
			&& !Username->text().isEmpty()
			&& !NewPassword->text().isEmpty()
			&& !ReNewPassword->text().isEmpty()
			&& !AccountManager::instance()->byId("jabber", Username->text() + '@' + Domain->currentText())
			&& IdentityCombo->currentIdentity();

	RegisterAccountButton->setEnabled(valid);

	if (Domain->currentText().isEmpty()
			&& Username->text().isEmpty()
			&& NewPassword->text().isEmpty()
			&& ReNewPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& !IdentityCombo->currentIdentity()
			&& !CustomHostPort->isChecked()
			&& CustomHost->text().isEmpty()
			&& CustomPort->text().toUInt() == port_
			&& EncryptionMode->currentIndex() == 1
			&& LegacySSLProbe->isChecked())
		setState(StateNotChanged);
	else
		setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void JabberCreateAccountWidget::apply()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Invalid data entered in required fields.\n\n"
			"Password entered in both fields (\"New password\" and \"Retype password\") "
			"must be the same!"), QMessageBox::Ok, this);
		return;
	}

	ssl_ = EncryptionMode->itemData(EncryptionMode->currentIndex()).toInt();
	legacy_ssl_probe_ = LegacySSLProbe->isChecked();
	opt_host_ = CustomHostPort->isChecked();
	host_ = CustomHost->text();
	port_ = CustomPort->text().toUInt();

	JabberServerRegisterAccount *jsra = new JabberServerRegisterAccount(Domain->currentText(), Username->text(), NewPassword->text(), legacy_ssl_probe_, ssl_ == 2, ssl_ == 0, opt_host_ ? host_ : QString(), port_);

	JabberWaitForAccountRegisterWindow *window = new JabberWaitForAccountRegisterWindow(jsra);
	connect(window, SIGNAL(jidRegistered(QString,QString)), this, SLOT(jidRegistered(QString,QString)));
	window->exec();
}

void JabberCreateAccountWidget::cancel()
{
	resetGui();
}

void JabberCreateAccountWidget::resetGui()
{
	Username->clear();
	Domain->setCurrentIndex(-1);
	NewPassword->clear();
	ReNewPassword->clear();
	RememberPassword->setChecked(true);
	IdentityCombo->setCurrentIdentity(Identity::null);
	ShowConnectionOptions = false;
	ExpandConnectionOptionsButton->setText(">");
	OptionsWidget->setVisible(false);
	CustomHost->setEnabled(false);
	CustomHostLabel->setEnabled(false);
	CustomPort->setEnabled(false);
	CustomPortLabel->setEnabled(false);
	EncryptionMode->setCurrentIndex(1);
	LegacySSLProbe->setChecked(true);
	RegisterAccountButton->setEnabled(false);

	IdentityManager::instance()->removeUnused();
	setState(StateNotChanged);
}

void JabberCreateAccountWidget::jidRegistered(const QString &jid, const QString &tlsDomain)
{
	if (jid.isEmpty())
	{
		emit accountCreated(Account());
		return;
	}

	Account jabberAccount = Account::create();
	jabberAccount.setProtocolName("jabber");
	jabberAccount.setAccountIdentity(IdentityCombo->currentIdentity());
	jabberAccount.setId(jid);
	jabberAccount.setHasPassword(true);
	jabberAccount.setPassword(NewPassword->text());
	jabberAccount.setRememberPassword(RememberPassword->isChecked());

	JabberAccountDetails *details = dynamic_cast<JabberAccountDetails *>(jabberAccount.details());
	if (details)
	{
		details->setState(StorableObject::StateNew);
		details->setTlsOverrideDomain(tlsDomain);
	}

	resetGui();

	emit accountCreated(jabberAccount);
}
