/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010, 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Pełzowski (floss@pelzowski.eu)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009, 2011 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtCrypto>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/account-avatar-widget.h"
#include "gui/widgets/account-buddy-list-widget.h"
#include "gui/widgets/account-configuration-widget-tab-adapter.h"
#include "gui/widgets/proxy-combo-box.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/services/avatar-service.h"

#include "gui/windows/jabber-change-password-window.h"

#include "jabber-edit-account-widget.h"

JabberEditAccountWidget::JabberEditAccountWidget(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository, Account account, QWidget *parent) :
		AccountEditWidget(accountConfigurationWidgetFactoryRepository, account, parent)
{
	createGui();
	loadAccountData();
	loadAccountDetailsData();
	simpleStateNotifier()->setState(StateNotChanged);
	stateChangedSlot(stateNotifier()->state());
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
	createConnectionTab(tabWidget);
	createOptionsTab(tabWidget);

	new AccountConfigurationWidgetTabAdapter(this, tabWidget, this);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	ApplyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	connect(ApplyButton, SIGNAL(clicked(bool)), this, SLOT(apply()));

	CancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(CancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	QPushButton *removeAccount = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Delete account"), this);
	connect(removeAccount, SIGNAL(clicked(bool)), this, SLOT(removeAccount()));

	buttons->addButton(ApplyButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(CancelButton, QDialogButtonBox::RejectRole);
	buttons->addButton(removeAccount, QDialogButtonBox::DestructiveRole);

	mainLayout->addWidget(buttons);

	connect(stateNotifier(), SIGNAL(stateChanged(ConfigurationValueState)), this, SLOT(stateChangedSlot(ConfigurationValueState)));
}

void JabberEditAccountWidget::createGeneralTab(QTabWidget *tabWidget)
{
	QWidget *generalTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(generalTab);
	QWidget *form = new QWidget(generalTab);
	layout->addWidget(form, 0, 0);

	QFormLayout *formLayout = new QFormLayout(form);

	AccountId = new QLineEdit(this);
	connect(AccountId, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Username") + ':', AccountId);

	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	connect(AccountPassword, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Password") + ':', AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	connect(RememberPassword, SIGNAL(clicked()), this, SLOT(dataChanged()));
	formLayout->addRow(0, RememberPassword);

	QLabel *changePasswordLabel = new QLabel(QString("<a href='change'>%1</a>").arg(tr("Change your password")));
	changePasswordLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	formLayout->addRow(0, changePasswordLabel);
	connect(changePasswordLabel, SIGNAL(linkActivated(QString)), this, SLOT(changePasssword()));

	Identities = new IdentitiesComboBox(this);
	connect(Identities, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Account Identity") + ':', Identities);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	formLayout->addRow(0, infoLabel);

	AccountAvatarWidget *avatarWidget = new AccountAvatarWidget(account(), this);
	layout->addWidget(avatarWidget, 0, 1, Qt::AlignTop);

	tabWidget->addTab(generalTab, tr("General"));
}

void JabberEditAccountWidget::createPersonalDataTab(QTabWidget *tabWidget)
{
	PersonalInfoWidget = new JabberPersonalInfoWidget(account(), tabWidget);
	connect(PersonalInfoWidget, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
	tabWidget->addTab(PersonalInfoWidget, tr("Personal Information"));
}

void JabberEditAccountWidget::createConnectionTab(QTabWidget *tabWidget)
{
	QWidget *conenctionTab = new QWidget(this);
	tabWidget->addTab(conenctionTab, tr("Connection"));

	QVBoxLayout *layout = new QVBoxLayout(conenctionTab);
	createGeneralGroupBox(layout);

	layout->addStretch(100);
}

void JabberEditAccountWidget::createGeneralGroupBox(QVBoxLayout *layout)
{
	QGroupBox *general = new QGroupBox(this);
	general->setTitle(tr("XMPP Server"));
	layout->addWidget(general);

	QFormLayout *boxLayout = new QFormLayout(general);
	boxLayout->setSpacing(6);
	boxLayout->setMargin(9);

	CustomHostPort = new QCheckBox(general);
	CustomHostPort->setText(tr("Use custom server address/port"));
	boxLayout->addRow(CustomHostPort);

	CustomHostLabel = new QLabel(general);
	CustomHostLabel->setText(tr("Server address") + ':');

	CustomHost = new QLineEdit(general);
	connect(CustomHost, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	boxLayout->addRow(CustomHostLabel, CustomHost);

	CustomPortLabel = new QLabel(general);
	CustomPortLabel->setText(tr("Port") + ':');

	CustomPort = new QLineEdit(general);
	CustomPort->setMinimumSize(QSize(56, 0));
	CustomPort->setMaximumSize(QSize(56, 32767));
	CustomPort->setValidator(new QIntValidator(0, 9999999, CustomPort));
	connect(CustomPort, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	boxLayout->addRow(CustomPortLabel, CustomPort);

	// Manual Host/Port
	CustomHost->setEnabled(false);
	CustomHostLabel->setEnabled(false);
	CustomPort->setEnabled(false);
	CustomPortLabel->setEnabled(false);
	connect(CustomHostPort, SIGNAL(toggled(bool)), SLOT(hostToggled(bool)));
	connect(CustomHostPort, SIGNAL(clicked()), this, SLOT(dataChanged()));

	EncryptionModeLabel = new QLabel(general);
	EncryptionModeLabel->setText(tr("Use encrypted connection") + ':');

	EncryptionMode = new QComboBox(general);
	EncryptionMode->addItem(tr("Never"), JabberAccountDetails::Encryption_No);
	EncryptionMode->addItem(tr("Always"), JabberAccountDetails::Encryption_Yes);
	EncryptionMode->addItem(tr("When available"), JabberAccountDetails::Encryption_Auto);
	EncryptionMode->addItem(tr("Only in older version"), JabberAccountDetails::Encryption_Legacy);
	connect(EncryptionMode, SIGNAL(activated(int)), SLOT(sslActivated(int)));
	connect(EncryptionMode, SIGNAL(activated(int)), this, SLOT(dataChanged()));
	boxLayout->addRow(EncryptionModeLabel, EncryptionMode);

	QLabel *plainAuthLabel = new QLabel(general);
	plainAuthLabel->setText(tr("Allow plaintext authentication") + ':');

	PlainTextAuth = new QComboBox(general);
	PlainTextAuth->addItem(tr("Never"), JabberAccountDetails::NoAllowPlain);
	PlainTextAuth->addItem(tr("Always"), JabberAccountDetails::AllowPlain);
	PlainTextAuth->addItem(tr("Over encrypted connection"), JabberAccountDetails::AllowPlainOverTLS);
	connect(PlainTextAuth, SIGNAL(activated(int)), this, SLOT(dataChanged()));
	boxLayout->addRow(plainAuthLabel, PlainTextAuth);


	QGroupBox *connection = new QGroupBox(this);
	connection->setTitle(tr("Network"));
	layout->addWidget(connection);

	QFormLayout *connectionBoxLayout = new QFormLayout(connection);
	boxLayout->setSpacing(6);
	boxLayout->setMargin(9);

	QLabel *dataTransferProxyLabel = new QLabel(connection);
	dataTransferProxyLabel->setText(tr("Data transfer proxy") + ':');

	DataTransferProxy = new QLineEdit(connection);
	connect(DataTransferProxy, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	connectionBoxLayout->addRow(dataTransferProxyLabel, DataTransferProxy);

	QLabel *proxyLabel = new QLabel(tr("Proxy configuration"), connection);
	ProxyCombo = new ProxyComboBox(connection);
	ProxyCombo->enableDefaultProxyAction();
	connect(ProxyCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));

	connectionBoxLayout->addRow(proxyLabel, ProxyCombo);
}

void JabberEditAccountWidget::createOptionsTab(QTabWidget *tabWidget)
{
	QWidget *optionsTab = new QWidget(this);
	tabWidget->addTab(optionsTab, tr("Options"));

	QVBoxLayout *layout = new QVBoxLayout(optionsTab);
	layout->setSpacing(6);
	layout->setMargin(9);

	QGroupBox *resource = new QGroupBox(tr("Resource"), this);
	QFormLayout *resourceLayout = new QFormLayout(resource);

	AutoResource = new QCheckBox(tr("Use computer name as a resource"));
	connect(AutoResource, SIGNAL(clicked()), this, SLOT(dataChanged()));
	connect(AutoResource, SIGNAL(toggled(bool)), SLOT(autoResourceToggled(bool)));
	resourceLayout->addRow(AutoResource);

	ResourceLabel = new QLabel;
	ResourceLabel->setText(tr("Resource") + ':');

	ResourceName = new QLineEdit;
	connect(ResourceName, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	resourceLayout->addRow(ResourceLabel, ResourceName);

	PriorityLabel = new QLabel;
	PriorityLabel->setText(tr("Priority") + ':');

	Priority = new QLineEdit;
	connect(Priority, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	Priority->setValidator(new QIntValidator(Priority));
	resourceLayout->addRow(PriorityLabel, Priority);

	layout->addWidget(resource);

	QGroupBox *options = new QGroupBox(tr("Options"), this);
	QFormLayout *optionsLayout = new QFormLayout(options);

	SendTypingNotification = new QCheckBox(tr("Enable composing events"));
	SendTypingNotification->setToolTip(tr("Your interlocutor will be notified when you are typing a message, before it is sent. And vice versa."));
	connect(SendTypingNotification, SIGNAL(clicked()), this, SLOT(dataChanged()));
	optionsLayout->addRow(SendTypingNotification);

	SendGoneNotification = new QCheckBox(tr("Enable chat activity events"));
	SendGoneNotification->setToolTip(tr("Your interlocutor will be notified when you suspend or end conversation. And vice versa."));
	SendGoneNotification->setEnabled(false);
	connect(SendGoneNotification, SIGNAL(clicked()), this, SLOT(dataChanged()));
	connect(SendTypingNotification, SIGNAL(toggled(bool)), SendGoneNotification, SLOT(setEnabled(bool)));
	optionsLayout->addRow(SendGoneNotification);

	PublishSystemInfo = new QCheckBox(tr("Publish system information"));
	PublishSystemInfo->setToolTip(tr("Others can see your system name/version"));
	connect(PublishSystemInfo, SIGNAL(clicked()), this, SLOT(dataChanged()));
	optionsLayout->addRow(PublishSystemInfo);

	layout->addWidget(options);

	layout->addStretch(100);
}

void JabberEditAccountWidget::hostToggled(bool on)
{
	CustomHost->setEnabled(on);
	CustomPort->setEnabled(on);
	CustomHostLabel->setEnabled(on);
	CustomPortLabel->setEnabled(on);
	if (!on && EncryptionMode->currentIndex() == EncryptionMode->findData(JabberAccountDetails::Encryption_Legacy)) {
		EncryptionMode->setCurrentIndex(JabberAccountDetails::Encryption_Auto);
	}
}

void JabberEditAccountWidget::autoResourceToggled(bool on)
{
	ResourceName->setEnabled(!on);
	ResourceLabel->setEnabled(!on);
}

bool JabberEditAccountWidget::checkSSL()
{
	if (!QCA::isSupported("tls"))
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Cannot enable secure connection. SSL/TLS plugin not found."));
		return false;
	}
	return true;
}

void JabberEditAccountWidget::sslActivated(int i)
{
	if ((EncryptionMode->itemData(i) == JabberAccountDetails::Encryption_Auto || EncryptionMode->itemData(i) == JabberAccountDetails::Encryption_Legacy) && !checkSSL())
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(JabberAccountDetails::Encryption_No));
	else if (EncryptionMode->itemData(i) == JabberAccountDetails::Encryption_Legacy && !CustomHostPort->isChecked())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Legacy SSL is only available in combination with manual host/port."));
		EncryptionMode->setCurrentIndex(EncryptionMode->findData(JabberAccountDetails::Encryption_Auto));
	}
}

void JabberEditAccountWidget::stateChangedSlot(ConfigurationValueState state)
{
	ApplyButton->setEnabled(state == StateChangedDataValid);
	CancelButton->setEnabled(state != StateNotChanged);
}

void JabberEditAccountWidget::dataChanged()
{
  	AccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!AccountDetails)
		return;

	ConfigurationValueState widgetsState = stateNotifier()->state();

	if (account().accountIdentity() == Identities->currentIdentity()
		&& account().id() == AccountId->text()
		&& account().rememberPassword() == RememberPassword->isChecked()
		&& account().password() == AccountPassword->text()
		&& account().proxy() == ProxyCombo->currentProxy()
		&& account().useDefaultProxy() == ProxyCombo->isDefaultProxySelected()
		&& AccountDetails->useCustomHostPort() == CustomHostPort->isChecked()
		&& AccountDetails->customHost() == CustomHost->displayText()
		&& AccountDetails->customPort() == CustomPort->displayText().toInt()
		&& AccountDetails->encryptionMode() == (JabberAccountDetails::EncryptionFlag)EncryptionMode->itemData(EncryptionMode->currentIndex()).toInt()
		&& AccountDetails->plainAuthMode() == (JabberAccountDetails::AllowPlainType)PlainTextAuth->itemData(PlainTextAuth->currentIndex()).toInt()
		&& AccountDetails->autoResource() == AutoResource->isChecked()
		&& AccountDetails->resource() == ResourceName->text()
		&& AccountDetails->priority() == Priority->text().toInt()
		&& AccountDetails->dataTransferProxy() == DataTransferProxy->text()
		&& AccountDetails->sendGoneNotification() == SendGoneNotification->isChecked()
		&& AccountDetails->sendTypingNotification() == SendTypingNotification->isChecked()
		&& AccountDetails->publishSystemInfo() == PublishSystemInfo->isChecked()
		&& !PersonalInfoWidget->isModified())
	{
		simpleStateNotifier()->setState(StateNotChanged);
		return;
	}

	bool sameIdExists = AccountManager::instance()->byId(account().protocolName(), AccountId->text())
			&& AccountManager::instance()->byId(account().protocolName(), AccountId->text()) != account();

	if (/*AccountName->text().isEmpty()
		|| sameNameExists
		|| */AccountId->text().isEmpty()
		|| sameIdExists
		|| StateChangedDataInvalid == widgetsState)
		simpleStateNotifier()->setState(StateChangedDataInvalid);
	else
		simpleStateNotifier()->setState(StateChangedDataValid);
}

void JabberEditAccountWidget::loadAccountData()
{
	Identities->setCurrentIdentity(account().accountIdentity());
	AccountId->setText(account().id());
	RememberPassword->setChecked(account().rememberPassword());
	AccountPassword->setText(account().password());

	if (account().useDefaultProxy())
		ProxyCombo->selectDefaultProxy();
	else
		ProxyCombo->setCurrentProxy(account().proxy());
}

void JabberEditAccountWidget::loadAccountDetailsData()
{
	AccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!AccountDetails)
		return;

	CustomHostPort->setChecked(AccountDetails->useCustomHostPort());
	CustomHost->setText(AccountDetails->customHost());
	CustomPort->setText(QString::number(AccountDetails->customPort()));
	EncryptionMode->setCurrentIndex(EncryptionMode->findData(AccountDetails->encryptionMode()));
	PlainTextAuth->setCurrentIndex(PlainTextAuth->findData(AccountDetails->plainAuthMode()));

	AutoResource->setChecked(AccountDetails->autoResource());
	ResourceName->setText(AccountDetails->resource());
	Priority->setText(QString::number(AccountDetails->priority()));
	DataTransferProxy->setText(AccountDetails->dataTransferProxy());

	SendGoneNotification->setChecked(AccountDetails->sendGoneNotification());
	SendTypingNotification->setChecked(AccountDetails->sendTypingNotification());

	PublishSystemInfo->setChecked(AccountDetails->publishSystemInfo());
}

void JabberEditAccountWidget::apply()
{
	AccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!AccountDetails)
		return;

	applyAccountConfigurationWidgets();

	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());
	account().setUseDefaultProxy(ProxyCombo->isDefaultProxySelected());
	account().setProxy(ProxyCombo->currentProxy());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	account().setAccountIdentity(Identities->currentIdentity());
	AccountDetails->setUseCustomHostPort(CustomHostPort->isChecked());
	AccountDetails->setCustomHost(CustomHost->text());
	AccountDetails->setCustomPort(CustomPort->text().toInt());
	AccountDetails->setEncryptionMode((JabberAccountDetails::EncryptionFlag)EncryptionMode->itemData(EncryptionMode->currentIndex()).toInt());
	AccountDetails->setPlainAuthMode((JabberAccountDetails::AllowPlainType)PlainTextAuth->itemData(PlainTextAuth->currentIndex()).toInt());
	AccountDetails->setAutoResource(AutoResource->isChecked());
	AccountDetails->setResource(ResourceName->text());
	AccountDetails->setPriority(Priority->text().toInt());
	AccountDetails->setDataTransferProxy(DataTransferProxy->text());
	AccountDetails->setSendGoneNotification(SendGoneNotification->isChecked());
	AccountDetails->setSendTypingNotification(SendTypingNotification->isChecked());
	AccountDetails->setPublishSystemInfo(PublishSystemInfo->isChecked());

	if (PersonalInfoWidget->isModified())
		PersonalInfoWidget->apply();

	IdentityManager::instance()->removeUnused();
	ConfigurationManager::instance()->flush();

	simpleStateNotifier()->setState(StateNotChanged);
}

void JabberEditAccountWidget::cancel()
{
	cancelAccountConfigurationWidgets();

	loadAccountData();
	loadAccountDetailsData();
	PersonalInfoWidget->cancel();

	IdentityManager::instance()->removeUnused();

	simpleStateNotifier()->setState(StateNotChanged);
}

void JabberEditAccountWidget::removeAccount()
{
	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-warning"), tr("Confrim Account Removal"),
	                        tr("Are you sure do you want to remove account %1 (%2)?")
				.arg(account().accountIdentity().name())
				.arg(account().id()));
	dialog->addButton(QMessageBox::Yes, tr("Remove account"));
	dialog->addButton(QMessageBox::Cancel, tr("Cancel"));
	dialog->setDefaultButton(QMessageBox::Cancel);
	int decision = dialog->exec();

	if (decision == QMessageBox::Yes)
	{
		AccountManager::instance()->removeAccountAndBuddies(account());
		deleteLater();
	}
}

void JabberEditAccountWidget::changePasssword()
{
	JabberChangePasswordWindow *changePasswordWindow = new JabberChangePasswordWindow(account());
	connect(changePasswordWindow, SIGNAL(passwordChanged(const QString &)), this, SLOT(passwordChanged(const QString &)));
	changePasswordWindow->show();
}

void JabberEditAccountWidget::passwordChanged(const QString &newPassword)
{
	AccountPassword->setText(newPassword);
}

#include "moc_jabber-edit-account-widget.cpp"
