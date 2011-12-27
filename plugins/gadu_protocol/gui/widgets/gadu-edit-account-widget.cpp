/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QWeakPointer>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/account-avatar-widget.h"
#include "gui/widgets/account-buddy-list-widget.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/widgets/proxy-combo-box.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"
#include "protocols/services/contact-list-service.h"

#include "gui/windows/gadu-change-password-window.h"
#include "gui/windows/gadu-remind-password-window.h"
#include "gui/windows/gadu-unregister-account-window.h"
#include "services/gadu-contact-list-service.h"
#include "gadu-account-details.h"
#include "gadu-id-validator.h"

#include "gadu-personal-info-widget.h"

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(Account account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	Details = dynamic_cast<GaduAccountDetails *>(account.details());

	createGui();
	loadAccountData();
	resetState();
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

	QLabel *remindPasswordLabel = new QLabel(QString("<a href='remind'>%1</a>").arg(tr("Forgot Your Password?")));
	remindPasswordLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	formLayout->addRow(0, remindPasswordLabel);
	connect(remindPasswordLabel, SIGNAL(linkActivated(QString)), this, SLOT(remindPasssword()));

	QLabel *changePasswordLabel = new QLabel(QString("<a href='change'>%1</a>").arg(tr("Change Your Password")));
	changePasswordLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	formLayout->addRow(0, changePasswordLabel);
	connect(changePasswordLabel, SIGNAL(linkActivated(QString)), this, SLOT(changePasssword()));

	Identities = new IdentitiesComboBox(false, this);
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

	QGroupBox *inclomingImages = new QGroupBox(tr("Incoming Images"), this);
	QFormLayout *incomingImagesLayout = new QFormLayout(inclomingImages);

	LimitImageSize = new QCheckBox(tr("Limit incoming images' size"), optionsTab);
	connect(LimitImageSize, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	incomingImagesLayout->addRow(LimitImageSize);

	MaximumImageSize = new QSpinBox(optionsTab);
	MaximumImageSize->setMinimum(0);
	MaximumImageSize->setMaximum(20*1024); // 20 MiB
	MaximumImageSize->setSingleStep(10);
	MaximumImageSize->setSuffix(" kB");
	MaximumImageSize->setToolTip(tr("Maximum images' size that you accept"));
	connect(MaximumImageSize, SIGNAL(valueChanged(int)), this, SLOT(dataChanged()));
	incomingImagesLayout->addRow(tr("Maximum incoming images' size") + ':', MaximumImageSize);

	ImageSizeAsk = new QCheckBox(tr("Ask for confirmation if an image's size exceeds the limit"), optionsTab);
	connect(ImageSizeAsk, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	incomingImagesLayout->addRow(ImageSizeAsk);

	connect(LimitImageSize, SIGNAL(toggled(bool)), MaximumImageSize, SLOT(setEnabled(bool)));
	connect(LimitImageSize, SIGNAL(toggled(bool)), ImageSizeAsk, SLOT(setEnabled(bool)));

	ReceiveImagesDuringInvisibility = new QCheckBox(tr("Receive images also when Invisible"), optionsTab);
	connect(ReceiveImagesDuringInvisibility, SIGNAL(clicked()), this, SLOT(dataChanged()));
	incomingImagesLayout->addRow(ReceiveImagesDuringInvisibility);

	MaximumImageRequests = new QSpinBox(optionsTab);
	MaximumImageRequests->setMinimum(1);
	MaximumImageRequests->setMaximum(60);
	MaximumImageRequests->setSingleStep(1);
	connect(MaximumImageRequests, SIGNAL(valueChanged(int)), this, SLOT(dataChanged()));
	incomingImagesLayout->addRow(tr("Limit numbers of images received per minute") + ':', MaximumImageRequests);

	layout->addWidget(inclomingImages);

	// outgoing images

	QGroupBox *outgoingImages = new QGroupBox(tr("Outgoing Images"), this);
	QFormLayout *outgoingImagesLayout = new QFormLayout(outgoingImages);

	ChatImageSizeWarning = new QCheckBox(tr("Show a warning when the image is larger then 256 KiB"), optionsTab);
	connect(ChatImageSizeWarning, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	outgoingImagesLayout->addRow(ChatImageSizeWarning);

	layout->addWidget(outgoingImages);

	QGroupBox *other = new QGroupBox(tr("Other"), this);
	QFormLayout *otherLayout = new QFormLayout(other);

	layout->addWidget(other);

	// status

	ShowStatusToEveryone = new QCheckBox(tr("Show my status to everyone"), other);
	ShowStatusToEveryone->setToolTip(tr("When disabled, you're visible only to buddies on your list"));
	connect(ShowStatusToEveryone, SIGNAL(clicked(bool)), this, SLOT(showStatusToEveryoneToggled(bool)));
	connect(ShowStatusToEveryone, SIGNAL(clicked()), this, SLOT(dataChanged()));

	otherLayout->addRow(ShowStatusToEveryone);

	// notifications

	SendTypingNotification = new QCheckBox(tr("Send composing events"), other);
	connect(SendTypingNotification, SIGNAL(clicked()), this, SLOT(dataChanged()));

	otherLayout->addRow(SendTypingNotification);

	// spam

	ReceiveSpam = new QCheckBox(tr("Receive URLs from anonymous buddies"), other);
	connect(ReceiveSpam, SIGNAL(clicked()), this, SLOT(dataChanged()));

	otherLayout->addRow(ReceiveSpam);

	// stretch

	layout->addStretch(100);
}

void GaduEditAccountWidget::createGeneralGroupBox(QVBoxLayout *layout)
{
	QGroupBox *general = new QGroupBox(tr("General"), this);
	QGridLayout *generalLayout = new QGridLayout(general);
	generalLayout->setColumnMinimumWidth(0, 20);
	generalLayout->setColumnMinimumWidth(3, 20);
	layout->addWidget(general);

	useDefaultServers = new QCheckBox(tr("Use default servers"), general);
	generalLayout->addWidget(useDefaultServers, 0, 0, 1, 4);

	QLabel *ipAddressesLabel = new QLabel(tr("IP addresses"), general);
	ipAddresses = new QLineEdit(general);
	ipAddresses->setToolTip("You can specify which servers and ports to use.\n"
							"Separate every server using semicolon.\n"
							"The last IPv4 octet may be specified as a range of addresses.\n"
							"For example:\n"
							"91.214.237.1 ; 91.214.237.3 ; 91.214.237.10:8074 ; 91.214.237.11-20 ; 91.214.237.21-30:8074");
	generalLayout->addWidget(ipAddressesLabel, 1, 1);
	generalLayout->addWidget(ipAddresses, 1, 2);

	AllowFileTransfers = new QCheckBox(tr("Allow file transfers"), general);
	generalLayout->addWidget(AllowFileTransfers, 2, 0, 1, 4);

	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddressesLabel, SLOT(setDisabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddresses, SLOT(setDisabled(bool)));

	connect(useDefaultServers, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	connect(ipAddresses, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	connect(AllowFileTransfers, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));

	UseTlsEncryption = new QCheckBox(tr("Use encrypted connection"), general);
	generalLayout->addWidget(UseTlsEncryption, 3, 0, 1, 4);

	if (gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL))
		connect(UseTlsEncryption, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	else
	{
		UseTlsEncryption->setDisabled(true);
		UseTlsEncryption->setToolTip(tr("You have to compile libgadu with SSL support to be able to enable encrypted connection"));
	}

	QHBoxLayout *externalLayout = new QHBoxLayout();

	ExternalIp = new QLineEdit(general);
	connect(ExternalIp, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));

	externalLayout->addWidget(new QLabel(tr("External ip") + ':', general));
	externalLayout->addWidget(ExternalIp);

	ExternalPort = new QLineEdit(general);
	ExternalPort->setValidator(new QIntValidator(0, 99999, ExternalPort));
	connect(ExternalPort, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));

	externalLayout->addWidget(new QLabel(tr("External port") + ':', general));
	externalLayout->addWidget(ExternalPort);

	generalLayout->addLayout(externalLayout, 4, 0, 1, 4);

	QLabel *proxyLabel = new QLabel(tr("Proxy configuration"), general);
	ProxyCombo = new ProxyComboBox(general);
	ProxyCombo->enableDefaultProxyAction();
	connect(ProxyCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));

	generalLayout->addWidget(proxyLabel, 5, 1);
	generalLayout->addWidget(ProxyCombo, 5, 2);
}

void GaduEditAccountWidget::apply()
{
	account().setAccountIdentity(Identities->currentIdentity());
	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());
	account().setPrivateStatus(!ShowStatusToEveryone->isChecked());
	account().setUseDefaultProxy(ProxyCombo->isDefaultProxySelected());
	account().setProxy(ProxyCombo->currentProxy());

	if (Details)
	{
		Details->setLimitImageSize(LimitImageSize->isChecked());
		Details->setMaximumImageSize(MaximumImageSize->value());
		Details->setImageSizeAsk(ImageSizeAsk->isChecked());
		Details->setReceiveImagesDuringInvisibility(ReceiveImagesDuringInvisibility->isChecked());
		Details->setMaximumImageRequests(MaximumImageRequests->value());

		Details->setChatImageSizeWarning(ChatImageSizeWarning->isChecked());

		Details->setAllowDcc(AllowFileTransfers->isChecked());
		if (gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL))
			Details->setTlsEncryption(UseTlsEncryption->isChecked());
		Details->setSendTypingNotification(SendTypingNotification->isChecked());
		Details->setReceiveSpam(ReceiveSpam->isChecked());

		Details->setExternalIp(ExternalIp->text());
		Details->setExternalPort(ExternalPort->text().toUInt());
	}

	config_file.writeEntry("Network", "isDefServers", useDefaultServers->isChecked());
	config_file.writeEntry("Network", "Server", ipAddresses->text());
	GaduServersManager::instance()->buildServerList();

	if (gpiw->isModified())
		gpiw->apply();

	IdentityManager::instance()->removeUnused();
	ConfigurationManager::instance()->flush();

	resetState();

	// TODO: 0.11, fix this
	// hack, changing details does not trigger this
	account().data()->emitUpdated();
}

void GaduEditAccountWidget::cancel()
{
	loadAccountData();
	gpiw->cancel();

	IdentityManager::instance()->removeUnused();

	resetState();
}

void GaduEditAccountWidget::resetState()
{
	setState(StateNotChanged);
	ApplyButton->setEnabled(false);
	CancelButton->setEnabled(false);
}

void GaduEditAccountWidget::dataChanged()
{
	if (account().accountIdentity() == Identities->currentIdentity()
		&& account().id() == AccountId->text()
		&& account().rememberPassword() == RememberPassword->isChecked()
		&& account().password() == AccountPassword->text()
		&& account().privateStatus() != ShowStatusToEveryone->isChecked()
		&& account().useDefaultProxy() == ProxyCombo->isDefaultProxySelected()
		&& account().proxy() == ProxyCombo->currentProxy()
		&& Details->limitImageSize() == LimitImageSize->isChecked()
		&& Details->maximumImageSize() == MaximumImageSize->value()
		&& Details->imageSizeAsk() == ImageSizeAsk->isChecked()
		&& Details->receiveImagesDuringInvisibility() == ReceiveImagesDuringInvisibility->isChecked()
		&& Details->maximumImageRequests() == MaximumImageRequests->value()

		&& Details->chatImageSizeWarning() == ChatImageSizeWarning->isChecked()

		&& Details->allowDcc() == AllowFileTransfers->isChecked()

		&& config_file.readBoolEntry("Network", "isDefServers", true) == useDefaultServers->isChecked()
		&& config_file.readEntry("Network", "Server") == ipAddresses->text()
		&& (!gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL) || Details->tlsEncryption() == UseTlsEncryption->isChecked())
		&& Details->sendTypingNotification() == SendTypingNotification->isChecked()
		&& Details->receiveSpam() == ReceiveSpam->isChecked()
		&& !gpiw->isModified()

		&& Details->externalIp() == ExternalIp->text()
		&& Details->externalPort() == ExternalPort->text().toUInt())
	{
		resetState();
		return;
	}

	bool sameIdExists = AccountManager::instance()->byId(account().protocolName(), AccountId->text())
			&& AccountManager::instance()->byId(account().protocolName(), AccountId->text()) != account();

	if (AccountId->text().isEmpty() || sameIdExists)
	{
		setState(StateChangedDataInvalid);
		ApplyButton->setEnabled(false);
		CancelButton->setEnabled(true);
	}
	else
	{
		setState(StateChangedDataValid);
		ApplyButton->setEnabled(true);
		CancelButton->setEnabled(true);
	}
}

void GaduEditAccountWidget::loadAccountData()
{
	Identities->setCurrentIdentity(account().accountIdentity());
	AccountId->setText(account().id());
	RememberPassword->setChecked(account().rememberPassword());
	AccountPassword->setText(account().password());
	ShowStatusToEveryone->setChecked(!account().privateStatus());
	if (account().useDefaultProxy())
		ProxyCombo->selectDefaultProxy();
	else
		ProxyCombo->setCurrentProxy(account().proxy());

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());
	if (details)
	{
		LimitImageSize->setChecked(details->limitImageSize());
		MaximumImageSize->setValue(details->maximumImageSize());
		ImageSizeAsk->setChecked(details->imageSizeAsk());
		ReceiveImagesDuringInvisibility->setChecked(details->receiveImagesDuringInvisibility());
		MaximumImageRequests->setValue(details->maximumImageRequests());
		MaximumImageSize->setEnabled(details->limitImageSize());
		ImageSizeAsk->setEnabled(details->limitImageSize());

		ChatImageSizeWarning->setChecked(details->chatImageSizeWarning());

		AllowFileTransfers->setChecked(details->allowDcc());
		UseTlsEncryption->setChecked(gg_libgadu_check_feature(GG_LIBGADU_FEATURE_SSL) ? details->tlsEncryption() : false);
		SendTypingNotification->setChecked(details->sendTypingNotification());
		ReceiveSpam->setChecked(details->receiveSpam());

		ExternalIp->setText(details->externalIp());
		ExternalPort->setText(QString::number(details->externalPort()));
	}

	useDefaultServers->setChecked(config_file.readBoolEntry("Network", "isDefServers", true));
	ipAddresses->setText(config_file.readEntry("Network", "Server"));
}

void GaduEditAccountWidget::removeAccount()
{
	QWeakPointer<QMessageBox> messageBox = new QMessageBox(this);
	messageBox.data()->setWindowTitle(tr("Confirm account removal"));
	messageBox.data()->setText(tr("Are you sure do you want to remove account %1 (%2)")
			.arg(account().accountIdentity().name())
			.arg(account().id()));

	QPushButton *removeButton = messageBox.data()->addButton(tr("Remove account"), QMessageBox::AcceptRole);
	QPushButton *removeAndUnregisterButton = messageBox.data()->addButton(tr("Remove account and unregister from server"), QMessageBox::DestructiveRole);
	messageBox.data()->addButton(QMessageBox::Cancel);
	messageBox.data()->setDefaultButton(QMessageBox::Cancel);
	messageBox.data()->exec();

	if (messageBox.isNull())
		return;

	if (messageBox.data()->clickedButton() == removeButton)
	{
		AccountManager::instance()->removeAccountAndBuddies(account());
		deleteLater();
	}
	else if (messageBox.data()->clickedButton() == removeAndUnregisterButton)
		(new GaduUnregisterAccountWindow(account()))->show();

	delete messageBox.data();
}

void GaduEditAccountWidget::remindPasssword()
{
	bool ok;
	UinType uin = AccountId->text().toUInt(&ok);
	if (ok)
		(new GaduRemindPasswordWindow(uin))->show();
}

void GaduEditAccountWidget::changePasssword()
{
	bool ok;
	UinType uin = AccountId->text().toUInt(&ok);
	if (ok)
	{
		GaduChangePasswordWindow *changePasswordWindow = new GaduChangePasswordWindow(uin, account());
		connect(changePasswordWindow, SIGNAL(passwordChanged(const QString &)), this, SLOT(passwordChanged(const QString &)));
		changePasswordWindow->show();
	}
}

void GaduEditAccountWidget::passwordChanged(const QString &newPassword)
{
	AccountPassword->setText(newPassword);
}

void GaduEditAccountWidget::showStatusToEveryoneToggled(bool toggled)
{
	if (!toggled)
		return;

	int count = 0;

	const QVector<Contact> &contacts = ContactManager::instance()->contacts(account());
	foreach (const Contact &contact, contacts)
		if (!contact.isAnonymous() && contact.ownerBuddy().isOfflineTo())
			count++;

	if (!count)
		return;

	QWeakPointer<QMessageBox> messageBox = new QMessageBox(this);
	messageBox.data()->setWindowTitle(tr("Confirm checking \"Show my status to everyone\" option"));
	messageBox.data()->setText(tr("Are you sure do you want to check \"Show my status to everyone\" option?\n"
	                              "You have several buddies which are not allowed to see your status.\n"
	                              "Enabling this option will allow them to know you are available."));
	QAbstractButton* yesButton = messageBox.data()->addButton(QMessageBox::Yes);
	messageBox.data()->addButton(QMessageBox::No);
	messageBox.data()->setDefaultButton(QMessageBox::No);
	messageBox.data()->exec();

	if (messageBox.isNull())
		return;

	if (messageBox.data()->clickedButton() == yesButton)
		return;

	ShowStatusToEveryone->setChecked(false);
}
