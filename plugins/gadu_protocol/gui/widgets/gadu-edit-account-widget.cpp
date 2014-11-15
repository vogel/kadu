/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "core/application.h"
#include "gui/widgets/account-avatar-widget.h"
#include "gui/widgets/account-buddy-list-widget.h"
#include "gui/widgets/account-configuration-widget-tab-adapter.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/widgets/proxy-combo-box.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "os/generic/url-opener.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"

#include "gadu-account-details.h"
#include "gadu-id-validator.h"

#include "gadu-personal-info-widget.h"

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository, Account account, QWidget *parent) :
		AccountEditWidget(accountConfigurationWidgetFactoryRepository, account, parent)
{
	Details = dynamic_cast<GaduAccountDetails *>(account.details());

	createGui();
	loadAccountData();
	stateChangedSlot(stateNotifier()->state());
}

GaduEditAccountWidget::~GaduEditAccountWidget()
{
}

void GaduEditAccountWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QTabWidget *tabWidget = new QTabWidget(this);
	mainLayout->addWidget(tabWidget);

	createGeneralTab(tabWidget);
	createPersonalInfoTab(tabWidget);
	createBuddiesTab(tabWidget);
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

void GaduEditAccountWidget::createGeneralTab(QTabWidget *tabWidget)
{
	QWidget *generalTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(generalTab);
	QWidget *form = new QWidget(generalTab);
	layout->addWidget(form, 0, 0);

	QFormLayout *formLayout = new QFormLayout(form);

	AccountId = new QLineEdit(this);
	AccountId->setValidator(GaduIdValidator::instance());
	connect(AccountId, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Gadu-Gadu number") + ':', AccountId);

	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	connect(AccountPassword, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Password") + ':', AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	connect(RememberPassword, SIGNAL(clicked()), this, SLOT(dataChanged()));
	formLayout->addRow(0, RememberPassword);

	auto remindUinLabel = new QLabel(QString("<a href='change'>%1</a>").arg(tr("Remind GG number")));
	remindUinLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	formLayout->addRow(0, remindUinLabel);
	connect(remindUinLabel, SIGNAL(linkActivated(QString)), this, SLOT(remindUin()));

	auto remindPasswordLabel = new QLabel(QString("<a href='change'>%1</a>").arg(tr("Remind Password")));
	remindPasswordLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	formLayout->addRow(0, remindPasswordLabel);
	connect(remindPasswordLabel, SIGNAL(linkActivated(QString)), this, SLOT(remindPassword()));

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

void GaduEditAccountWidget::createPersonalInfoTab(QTabWidget *tabWidget)
{
	gpiw = new GaduPersonalInfoWidget(account(), tabWidget);
	connect(gpiw, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
	tabWidget->addTab(gpiw, tr("Personal info"));
}

void GaduEditAccountWidget::createBuddiesTab(QTabWidget *tabWidget)
{
	QWidget *widget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(widget);

	AccountBuddyListWidget *buddiesWidget = new AccountBuddyListWidget(account(), widget);
	layout->addWidget(buddiesWidget);

	tabWidget->addTab(widget, tr("Buddies"));
}

void GaduEditAccountWidget::createConnectionTab(QTabWidget *tabWidget)
{
	QWidget *conenctionTab = new QWidget(this);
	tabWidget->addTab(conenctionTab, tr("Connection"));

	QVBoxLayout *layout = new QVBoxLayout(conenctionTab);
	createGeneralGroupBox(layout);

	layout->addStretch(100);
}

void GaduEditAccountWidget::createOptionsTab(QTabWidget *tabWidget)
{
	QWidget *optionsTab = new QWidget(this);
	tabWidget->addTab(optionsTab, tr("Options"));

	QVBoxLayout *layout = new QVBoxLayout(optionsTab);

	// incoming images

	QGroupBox *images = new QGroupBox(tr("Images"), this);
	QFormLayout *imagesLayout = new QFormLayout(images);

	ReceiveImagesDuringInvisibility = new QCheckBox(tr("Receive images also when I am Invisible"), optionsTab);
	connect(ReceiveImagesDuringInvisibility, SIGNAL(clicked()), this, SLOT(dataChanged()));
	imagesLayout->addRow(ReceiveImagesDuringInvisibility);

	ChatImageSizeWarning = new QCheckBox(tr("Warn me when the image being sent may be too large"), optionsTab);
	ChatImageSizeWarning->setToolTip(tr("Some clients may have trouble with too large images (over 256 KiB)."));
	connect(ChatImageSizeWarning, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	imagesLayout->addRow(ChatImageSizeWarning);

	layout->addWidget(images);

	QGroupBox *other = new QGroupBox(tr("Other"), this);
	QFormLayout *otherLayout = new QFormLayout(other);

	layout->addWidget(other);

	// status

	ShowStatusToEveryone = new QCheckBox(tr("Show my status only to buddies on my list"), other);
	ShowStatusToEveryone->setToolTip(tr("When disabled, anyone can see your status."));
	connect(ShowStatusToEveryone, SIGNAL(clicked(bool)), this, SLOT(showStatusToEveryoneToggled(bool)));
	connect(ShowStatusToEveryone, SIGNAL(clicked()), this, SLOT(dataChanged()));

	otherLayout->addRow(ShowStatusToEveryone);

	// notifications

	SendTypingNotification = new QCheckBox(tr("Enable composing events"), other);
	SendTypingNotification->setToolTip(tr("Your interlocutor will be notified when you are typing a message, before it is sent. And vice versa."));
	connect(SendTypingNotification, SIGNAL(clicked()), this, SLOT(dataChanged()));

	otherLayout->addRow(SendTypingNotification);

	// spam

	ReceiveSpam = new QCheckBox(tr("Block links from anonymous buddies"), other);
	ReceiveSpam->setToolTip(tr("Protects you from potentially malicious links in messages from anonymous buddies"));
	connect(ReceiveSpam, SIGNAL(clicked()), this, SLOT(dataChanged()));

	otherLayout->addRow(ReceiveSpam);

	// stretch

	layout->addStretch(100);
}

void GaduEditAccountWidget::createGeneralGroupBox(QVBoxLayout *layout)
{
	QGroupBox *general = new QGroupBox(tr("Gadu-Gadu Server"), this);
	QFormLayout *generalLayout = new QFormLayout(general);

	useDefaultServers = new QCheckBox(tr("Use default servers"), general);
	generalLayout->addRow(useDefaultServers);

	QLabel *ipAddressesLabel = new QLabel(tr("Custom server IP addresses"), general);
	ipAddresses = new QLineEdit(general);
	ipAddresses->setToolTip("You can specify which servers and ports to use.\n"
							"Separate every server using semicolon.\n"
							"The last IPv4 octet may be specified as a range of addresses.\n"
							"For example:\n"
							"91.214.237.1 ; 91.214.237.3 ; 91.214.237.10:8074 ; 91.214.237.11-20 ; 91.214.237.21-30:8074");
	generalLayout->addRow(ipAddressesLabel, ipAddresses);

	AllowFileTransfers = new QCheckBox(tr("Enable file transfers"), general);
	generalLayout->addRow(AllowFileTransfers);

	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddressesLabel, SLOT(setDisabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddresses, SLOT(setDisabled(bool)));

	connect(useDefaultServers, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	connect(ipAddresses, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	connect(AllowFileTransfers, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));

	UseTlsEncryption = new QCheckBox(tr("Use encrypted connection"), general);
	generalLayout->addRow(UseTlsEncryption);

	if (gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL))
		connect(UseTlsEncryption, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	else
	{
		UseTlsEncryption->setDisabled(true);
		UseTlsEncryption->setToolTip(tr("You have to compile libgadu with SSL support to be able to enable encrypted connection"));
	}

	QGroupBox *connection = new QGroupBox(tr("Network"), this);
	QFormLayout *connectionLayout = new QFormLayout(connection);

	ExternalIp = new QLineEdit(connection);
	connect(ExternalIp, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));

	connectionLayout->addRow(new QLabel(tr("External IP") + ':', connection), ExternalIp);

	ExternalPort = new QLineEdit(connection);
	ExternalPort->setValidator(new QIntValidator(0, 99999, ExternalPort));
	connect(ExternalPort, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));

	connectionLayout->addRow(new QLabel(tr("External port") + ':', connection), ExternalPort);

	QLabel *proxyLabel = new QLabel(tr("Proxy configuration") + ':', connection);
	ProxyCombo = new ProxyComboBox(connection);
	ProxyCombo->enableDefaultProxyAction();
	connect(ProxyCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));

	connectionLayout->addRow(proxyLabel, ProxyCombo);

	layout->addWidget(general);
	layout->addWidget(connection);
}

void GaduEditAccountWidget::stateChangedSlot(ConfigurationValueState state)
{
	ApplyButton->setEnabled(state == StateChangedDataValid);
	CancelButton->setEnabled(state != StateNotChanged);
}

void GaduEditAccountWidget::apply()
{
	applyAccountConfigurationWidgets();

	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());
	account().setPrivateStatus(ShowStatusToEveryone->isChecked());
	account().setUseDefaultProxy(ProxyCombo->isDefaultProxySelected());
	account().setProxy(ProxyCombo->currentProxy());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	account().setAccountIdentity(Identities->currentIdentity());

	if (Details)
	{
		Details->setReceiveImagesDuringInvisibility(ReceiveImagesDuringInvisibility->isChecked());

		Details->setChatImageSizeWarning(ChatImageSizeWarning->isChecked());

		Details->setAllowDcc(AllowFileTransfers->isChecked());
		if (gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL))
			Details->setTlsEncryption(UseTlsEncryption->isChecked());
		Details->setSendTypingNotification(SendTypingNotification->isChecked());
		Details->setReceiveSpam(!ReceiveSpam->isChecked());

		Details->setExternalIp(ExternalIp->text());
		Details->setExternalPort(ExternalPort->text().toUInt());
	}

	Application::instance()->configuration()->deprecatedApi()->writeEntry("Network", "isDefServers", useDefaultServers->isChecked());
	Application::instance()->configuration()->deprecatedApi()->writeEntry("Network", "Server", ipAddresses->text());
	GaduServersManager::instance()->buildServerList();

	if (gpiw->isModified())
		gpiw->apply();

	IdentityManager::instance()->removeUnused();
	ConfigurationManager::instance()->flush();

	simpleStateNotifier()->setState(StateNotChanged);

	// TODO: 0.13, fix this
	// hack, changing details does not trigger this
	account().data()->forceEmitUpdated();
}

void GaduEditAccountWidget::cancel()
{
	cancelAccountConfigurationWidgets();

	loadAccountData();
	gpiw->cancel();

	IdentityManager::instance()->removeUnused();

	simpleStateNotifier()->setState(StateNotChanged);
}

void GaduEditAccountWidget::dataChanged()
{
	ConfigurationValueState widgetsState = stateNotifier()->state();

	if (account().accountIdentity() == Identities->currentIdentity()
		&& account().id() == AccountId->text()
		&& account().rememberPassword() == RememberPassword->isChecked()
		&& account().password() == AccountPassword->text()
		&& account().privateStatus() == ShowStatusToEveryone->isChecked()
		&& account().useDefaultProxy() == ProxyCombo->isDefaultProxySelected()
		&& account().proxy() == ProxyCombo->currentProxy()
		&& Details->receiveImagesDuringInvisibility() == ReceiveImagesDuringInvisibility->isChecked()

		&& Details->chatImageSizeWarning() == ChatImageSizeWarning->isChecked()

		&& Details->allowDcc() == AllowFileTransfers->isChecked()

		&& Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Network", "isDefServers", true) == useDefaultServers->isChecked()
		&& Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "Server") == ipAddresses->text()
		&& (!gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL) || Details->tlsEncryption() == UseTlsEncryption->isChecked())
		&& Details->sendTypingNotification() == SendTypingNotification->isChecked()
		&& Details->receiveSpam() != ReceiveSpam->isChecked()
		&& !gpiw->isModified()

		&& Details->externalIp() == ExternalIp->text()
		&& Details->externalPort() == ExternalPort->text().toUInt())
	{
		simpleStateNotifier()->setState(StateNotChanged);
		return;
	}

	bool sameIdExists = AccountManager::instance()->byId(account().protocolName(), AccountId->text())
			&& AccountManager::instance()->byId(account().protocolName(), AccountId->text()) != account();

	if (AccountId->text().isEmpty() || sameIdExists || StateChangedDataInvalid == widgetsState)
		simpleStateNotifier()->setState(StateChangedDataInvalid);
	else
		simpleStateNotifier()->setState(StateChangedDataValid);
}

void GaduEditAccountWidget::loadAccountData()
{
	Identities->setCurrentIdentity(account().accountIdentity());
	AccountId->setText(account().id());
	RememberPassword->setChecked(account().rememberPassword());
	AccountPassword->setText(account().password());
	ShowStatusToEveryone->setChecked(account().privateStatus());
	if (account().useDefaultProxy())
		ProxyCombo->selectDefaultProxy();
	else
		ProxyCombo->setCurrentProxy(account().proxy());

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());
	if (details)
	{
		ReceiveImagesDuringInvisibility->setChecked(details->receiveImagesDuringInvisibility());;

		ChatImageSizeWarning->setChecked(details->chatImageSizeWarning());

		AllowFileTransfers->setChecked(details->allowDcc());
		UseTlsEncryption->setChecked(gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL) ? details->tlsEncryption() : false);
		SendTypingNotification->setChecked(details->sendTypingNotification());
		ReceiveSpam->setChecked(!details->receiveSpam());

		ExternalIp->setText(details->externalIp());
		ExternalPort->setText(QString::number(details->externalPort()));
	}

	useDefaultServers->setChecked(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Network", "isDefServers", true));
	ipAddresses->setText(Application::instance()->configuration()->deprecatedApi()->readEntry("Network", "Server"));

	simpleStateNotifier()->setState(StateNotChanged);
}

void GaduEditAccountWidget::removeAccount()
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

void GaduEditAccountWidget::remindUin()
{
	UrlOpener::openUrl("https://login.gg.pl/account/remindGG_email/?id=frame_1");
}

void GaduEditAccountWidget::remindPassword()
{
	UrlOpener::openUrl("https://login.gg.pl/account/remindPassword/?id=frame_1");
}

void GaduEditAccountWidget::showStatusToEveryoneToggled(bool toggled)
{
	if (toggled)
		return;

	int count = 0;

	const QVector<Contact> &contacts = ContactManager::instance()->contacts(account());
	foreach (const Contact &contact, contacts)
		if (!contact.isAnonymous() && contact.ownerBuddy().isOfflineTo())
			count++;

	if (!count)
		return;

	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-warning"), tr("Status Visibility"),
	                        tr("You are going to reveal your status to several buddies which are currently not allowed to see it.\n"
				  "Are you sure to allow them to know you are available?"));
	dialog->addButton(QMessageBox::Yes, tr("Make my status visible anyway"));
	dialog->addButton(QMessageBox::No, tr("Stay with private status"));
	dialog->setDefaultButton(QMessageBox::No);
	int decision = dialog->exec();

	if (decision == QMessageBox::Yes)
		return;

	ShowStatusToEveryone->setChecked(false);
}

#include "moc_gadu-edit-account-widget.cpp"
