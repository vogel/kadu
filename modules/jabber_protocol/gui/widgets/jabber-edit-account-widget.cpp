/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCrypto>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFileDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/account-avatar-widget.h"
#include "gui/widgets/account-buddy-list-widget.h"
#include "gui/widgets/choose-identity-widget.h"
#include "gui/widgets/proxy-group-box.h"
#include "gui/windows/message-dialog.h"
#include "protocols/services/avatar-service.h"
#include "icons-manager.h"

#include "jabber-account-details.h"
#include "jabber-personal-info-widget.h"

#include "jabber-edit-account-widget.h"

JabberEditAccountWidget::JabberEditAccountWidget(Account account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	createGui();
	loadAccountData();
	loadConnectionData();
}

JabberEditAccountWidget::~JabberEditAccountWidget()
{
}

void JabberEditAccountWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QTabWidget *tabWidget = new QTabWidget(this);
	mainLayout->addWidget(tabWidget);

	createGeneralTab(tabWidget);
	createPersonalDataTab(tabWidget);
	createBuddiesTab(tabWidget);
	createConnectionTab(tabWidget);
	createOptionsTab(tabWidget);
}


void JabberEditAccountWidget::createGeneralTab(QTabWidget *tabWidget)
{
	QWidget *generalTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(generalTab);
	QWidget *form = new QWidget(generalTab);
	layout->addWidget(form, 0, 0);

	QFormLayout *formLayout = new QFormLayout(form);

	ConnectAtStart = new QCheckBox(tr("Connect at start"), this);
	formLayout->addRow(0, ConnectAtStart);

	AccountId = new QLineEdit(this);
	formLayout->addRow(tr("XMPP/Jabber Id") + ":", AccountId);

	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	formLayout->addRow(tr("Password") + ":", AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	formLayout->addRow(0, RememberPassword);

	ChooseIdentity = new ChooseIdentityWidget(this);
	formLayout->addRow(tr("Account Identity") + ":", ChooseIdentity);

	formLayout->addRow(0, new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this));

	QPushButton *changePassword = new QPushButton(tr("Change password"), this);
	formLayout->addRow(0, changePassword);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *applyButton = new QPushButton(IconsManager::instance()->iconByPath("16x16/dialog-apply.png"), tr("Apply"), this);
	connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(apply()));

	QPushButton *removeAccount = new QPushButton(IconsManager::instance()->iconByPath("16x16/dialog-cancel.png"), tr("Delete account"), this);
	connect(removeAccount, SIGNAL(clicked(bool)), this, SLOT(removeAccount()));

	buttons->addButton(applyButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(removeAccount, QDialogButtonBox::DestructiveRole);
	layout->addWidget(buttons, 1, 0, 1, 2);

	AccountAvatarWidget *avatarWidget = new AccountAvatarWidget(account(), this);
	layout->addWidget(avatarWidget, 0, 1, Qt::AlignTop);

	tabWidget->addTab(generalTab, tr("General"));
}

void JabberEditAccountWidget::createPersonalDataTab(QTabWidget *tabWidget)
{
	JabberPersonalInfoWidget *gpiw = new JabberPersonalInfoWidget(account(), tabWidget);
	tabWidget->addTab(gpiw, tr("Personal info"));
}

void JabberEditAccountWidget::createBuddiesTab(QTabWidget *tabWidget)
{
	AccountBuddyListWidget *widget = new AccountBuddyListWidget(account(), this);
	tabWidget->addTab(widget, tr("Buddies"));
}

void JabberEditAccountWidget::createConnectionTab(QTabWidget *tabWidget)
{
	QWidget *conenctionTab = new QWidget(this);
	tabWidget->addTab(conenctionTab, tr("Connection"));

	QVBoxLayout *layout = new QVBoxLayout(conenctionTab);
	createGeneralGroupBox(layout);

	proxy = new ProxyGroupBox(account(), tr("Proxy"), this);
	layout->addWidget(proxy);

	layout->addStretch(100);
}

void JabberEditAccountWidget::createGeneralGroupBox(QVBoxLayout *layout)
{
	QGroupBox *general = new QGroupBox(this);
	general->setTitle(tr("General"));
	layout->addWidget(general);

	QVBoxLayout *vboxLayout2 = new QVBoxLayout(general);
	vboxLayout2->setSpacing(6);
	vboxLayout2->setMargin(9);

	CustomHostPort = new QCheckBox(general);
	CustomHostPort->setText(tr("Manually Specify Server Host/Port")+":");
	vboxLayout2->addWidget(CustomHostPort);

	HostPortLayout = new QHBoxLayout();
	HostPortLayout->setSpacing(6);
	HostPortLayout->setMargin(0);

	CustomHostLabel = new QLabel(general);
	CustomHostLabel->setText(tr("Host")+":");
	HostPortLayout->addWidget(CustomHostLabel);

	CustomHost = new QLineEdit(general);
	HostPortLayout->addWidget(CustomHost);

	CustomPortLabel = new QLabel(general);
	CustomPortLabel->setText(tr("Port")+":");
	HostPortLayout->addWidget(CustomPortLabel);

	CustomPort = new QLineEdit(general);
	CustomPort->setMinimumSize(QSize(56, 0));
	CustomPort->setMaximumSize(QSize(56, 32767));
	CustomPort->setValidator(new QIntValidator(0, 9999999, CustomPort));
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
	EncryptionModeLabel = new QLabel(general);
	EncryptionModeLabel->setText(tr("Encrypt connection")+":");
	EncryptionLayout->addWidget(EncryptionModeLabel);

	EncryptionMode = new QComboBox(general);
	EncryptionMode->addItem(tr("Never"), JabberAccountDetails::Encryption_No);
	EncryptionMode->addItem(tr("Always"), JabberAccountDetails::Encryption_Yes);
	EncryptionMode->addItem(tr("When available"), JabberAccountDetails::Encryption_Auto);
	EncryptionMode->addItem(tr("Legacy SSL"), JabberAccountDetails::Encryption_Legacy);
	connect(EncryptionMode, SIGNAL(activated(int)), SLOT(sslActivated(int)));
	EncryptionLayout->addWidget(EncryptionMode);

	QSpacerItem *spacerItem = new QSpacerItem(151, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	EncryptionLayout->addItem(spacerItem);
	vboxLayout2->addLayout(EncryptionLayout);

	LegacySSLProbe = new QCheckBox(general);
	LegacySSLProbe->setText(tr("Probe legacy SSL port"));
	vboxLayout2->addWidget(LegacySSLProbe);

	QHBoxLayout *plainAuthLayout = new QHBoxLayout();
	plainAuthLayout->setSpacing(6);
	plainAuthLayout->setMargin(0);
	QLabel *plainAuthLabel = new QLabel(general);
	plainAuthLabel->setText(tr("Allow plaintext authentication")+":");
	plainAuthLayout->addWidget(plainAuthLabel);

	//TODO: powinno być XMPP::ClientStream::AllowPlainType - potrzebna koncepcja jak to zapisywać w konfiguracji
	PlainTextAuth = new QComboBox(general);
	PlainTextAuth->addItem(tr("Never"), JabberAccountDetails::Encryption_No);
	PlainTextAuth->addItem(tr("Always"), JabberAccountDetails::Encryption_Yes);
	PlainTextAuth->addItem(tr("When available"), JabberAccountDetails::Encryption_Auto);
	PlainTextAuth->addItem(tr("Legacy SSL"), JabberAccountDetails::Encryption_Legacy);
	plainAuthLayout->addWidget(PlainTextAuth);
	vboxLayout2->addLayout(plainAuthLayout);

}

void JabberEditAccountWidget::createOptionsTab(QTabWidget *tabWidget)
{
	QWidget *optionsTab = new QWidget(this);
	tabWidget->addTab(optionsTab, tr("Options"));

	QVBoxLayout *layout = new QVBoxLayout(optionsTab);
	layout->setSpacing(6);
	layout->setMargin(9);

	AutoResource = new QCheckBox;
	AutoResource->setText(tr("Use hostname as a resource"));
	layout->addWidget(AutoResource);

	ResourceLayout = new QHBoxLayout();
	ResourceLayout->setSpacing(6);
	ResourceLayout->setMargin(0);

	ResourceLabel = new QLabel;
	ResourceLabel->setText(tr("Resource")+":");
	ResourceLayout->addWidget(ResourceLabel);

	ResourceName = new QLineEdit;
	ResourceLayout->addWidget(ResourceName);

	PriorityLabel = new QLabel;
	PriorityLabel->setText(tr("Priority")+":");
	ResourceLayout->addWidget(PriorityLabel);

	Priority = new QLineEdit;
//	 Priority->setMinimumSize(QSize(56, 0));
//	 Priority->setMaximumSize(QSize(56, 32767));
	Priority->setValidator(new QIntValidator(Priority));
	ResourceLayout->addWidget(Priority);

/*	ResourceName->setEnabled(false);
	ResourceLabel->setEnabled(false);
	Priority->setEnabled(false);
	PriorityLabel->setEnabled(false);
*/
	connect(AutoResource, SIGNAL(toggled(bool)), SLOT(autoResourceToggled(bool)));

	layout->addLayout(ResourceLayout);
	
	DataTransferProxyLayout = new QHBoxLayout();
	DataTransferProxyLayout->setSpacing(6);
	DataTransferProxyLayout->setMargin(0);

	DataTransferProxyLabel = new QLabel;
	DataTransferProxyLabel->setText(tr("Data transfer proxy")+":");
	DataTransferProxyLayout->addWidget(DataTransferProxyLabel);

	DataTransferProxy = new QLineEdit;
	DataTransferProxyLayout->addWidget(DataTransferProxy);

	layout->addLayout(DataTransferProxyLayout);
}

void JabberEditAccountWidget::hostToggled(bool on)
{
	CustomHost->setEnabled(on);
	CustomPort->setEnabled(on);
	CustomHostLabel->setEnabled(on);
	CustomPortLabel->setEnabled(on);
	if (!on && EncryptionMode->currentIndex() == EncryptionMode->findData(2)) {
		EncryptionMode->setCurrentIndex(1);
	}
}

void JabberEditAccountWidget::autoResourceToggled(bool on)
{
	ResourceName->setEnabled(!on);
	ResourceLabel->setEnabled(!on);
}

bool JabberEditAccountWidget::checkSSL()
{
	if(!QCA::isSupported("tls")) {
		MessageDialog::msg(tr("Cannot enable SSL/TLS. Plugin not found."));
		return false;
	}
	return true;
}

void JabberEditAccountWidget::sslActivated(int i)
{
	if ((EncryptionMode->itemData(i) == 0 || EncryptionMode->itemData(i) == 2) && !checkSSL()) {
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(1));
	}
	else if (EncryptionMode->itemData(i) == 2 && !CustomHostPort->isChecked()) {
		MessageDialog::msg(tr("Legacy SSL is only available in combination with manual host/port."));
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(1));
	}
}

void JabberEditAccountWidget::loadAccountData()
{
	ConnectAtStart->setChecked(account().connectAtStart());
	AccountId->setText(account().id());
	RememberPassword->setChecked(account().rememberPassword());
	AccountPassword->setText(account().password());
}

void JabberEditAccountWidget::loadConnectionData()
{
	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
		return;

	CustomHostPort->setChecked(jabberAccountDetails->useCustomHostPort());
	CustomHost->setText(jabberAccountDetails->customHost());
	CustomPort->setText(jabberAccountDetails->customPort() ? QString::number(jabberAccountDetails->customPort()) : QString::number(5222));
	EncryptionMode->setCurrentIndex(EncryptionMode->findData(jabberAccountDetails->encryptionMode()));
	LegacySSLProbe->setChecked(jabberAccountDetails->legacySSLProbe());
	proxy->loadProxyData();

	AutoResource->setChecked(jabberAccountDetails->autoResource());
	ResourceName->setText(jabberAccountDetails->resource());
	Priority->setText(QString::number(jabberAccountDetails->priority()));
	DataTransferProxy->setText(jabberAccountDetails->dataTransferProxy());
}

void JabberEditAccountWidget::apply()
{
	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
		return;

	account().setConnectAtStart(ConnectAtStart->isChecked());
	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());
	jabberAccountDetails->setUseCustomHostPort(CustomHostPort->isChecked());
	jabberAccountDetails->setCustomHost(CustomHost->text());
	jabberAccountDetails->setCustomPort(CustomPort->text().toInt());
	jabberAccountDetails->setEncryptionMode((JabberAccountDetails::EncryptionFlag)EncryptionMode->itemData(EncryptionMode->currentIndex()).toInt());
	jabberAccountDetails->setLegacySSLProbe(LegacySSLProbe->isChecked());
	jabberAccountDetails->setAutoResource(AutoResource->isChecked());
	jabberAccountDetails->setResource(ResourceName->text());
	jabberAccountDetails->setPriority(Priority->text().toInt());
	jabberAccountDetails->setDataTransferProxy(DataTransferProxy->text());
}

void JabberEditAccountWidget::cancel()
{
}

void JabberEditAccountWidget::removeAccount()
{
	QMessageBox *messageBox = new QMessageBox(this);
	messageBox->setWindowTitle(tr("Confirm account removal"));
	messageBox->setText(tr("Are you sure do you want to remove account %1 (%2)")
			.arg(account().name())
			.arg(account().id()));

	QPushButton *removeButton = messageBox->addButton(tr("Remove account"), QMessageBox::AcceptRole);
	QPushButton *removeAndUnregisterButton = messageBox->addButton(tr("Remove account and unregister from server"), QMessageBox::DestructiveRole);
	messageBox->addButton(QMessageBox::Cancel);

	messageBox->exec();

	if (messageBox->clickedButton() == removeButton)
	{
		AccountManager::instance()->removeItem(account());
		deleteLater();
	}
	else if (messageBox->clickedButton() == removeAndUnregisterButton)
	{
		AccountManager::instance()->removeItem(account());
		deleteLater();
	}

	delete messageBox;
}

