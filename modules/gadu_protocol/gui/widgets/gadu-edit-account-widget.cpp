/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFileDialog>
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/account-avatar-widget.h"
#include "gui/widgets/account-buddy-list-widget.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/widgets/proxy-group-box.h"
#include "identities/identity-manager.h"
#include "protocols/services/avatar-service.h"
#include "protocols/services/contact-list-service.h"
#include "protocols/protocol.h"
#include "qt/long-validator.h"
#include "icons-manager.h"

#include "gui/windows/gadu-change-password-window.h"
#include "gui/windows/gadu-remind-password-window.h"
#include "gui/windows/gadu-unregister-account-window.h"
#include "services/gadu-contact-list-service.h"
#include "gadu-account-details.h"

#include "gadu-personal-info-widget.h"

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(Account account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	Details = dynamic_cast<GaduAccountDetails *>(account.details());

	createGui();
	loadAccountData();
	loadConnectionData();
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
	AccountId->setValidator(new LongValidator(1, 3999999999U, this));
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
	connect(Identities, SIGNAL(identityChanged(Identity)), this, SLOT(dataChanged()));
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

	Proxy = new ProxyGroupBox(account(), tr("Proxy"), this);
	connect(Proxy, SIGNAL(stateChanged(ModalConfigurationWidgetState)), this, SLOT(dataChanged()));
	layout->addWidget(Proxy);

	layout->addStretch(100);
}

void GaduEditAccountWidget::createOptionsTab(QTabWidget *tabWidget)
{
	QWidget *optionsTab = new QWidget(this);
	tabWidget->addTab(optionsTab, tr("Options"));

	QVBoxLayout *layout = new QVBoxLayout(optionsTab);

	QGroupBox *images = new QGroupBox(tr("Images"), this);
	QFormLayout *imagesLayout = new QFormLayout(images);

	MaximumImageSize = new QSpinBox(optionsTab);
	MaximumImageSize->setMinimum(0);
	MaximumImageSize->setMaximum(255);
	MaximumImageSize->setSingleStep(10);
	MaximumImageSize->setSuffix(" kB");
	MaximumImageSize->setToolTip(tr("Maximum image size that we want to receive"));
	connect(MaximumImageSize, SIGNAL(valueChanged(int)), this, SLOT(dataChanged()));

	imagesLayout->addRow(tr("Maximum image size for chat") + ':', MaximumImageSize);

	ReceiveImagesDuringInvisibility = new QCheckBox(tr("Receive images during invisibility"), optionsTab);
	ReceiveImagesDuringInvisibility->setToolTip(tr("Receiving images during invisibility is allowed"));
	connect(ReceiveImagesDuringInvisibility, SIGNAL(clicked()), this, SLOT(dataChanged()));

	imagesLayout->addRow(ReceiveImagesDuringInvisibility);

	MaximumImageRequests = new QSpinBox(optionsTab);
	MaximumImageRequests->setMinimum(1);
	MaximumImageRequests->setMaximum(30);
	MaximumImageRequests->setSingleStep(1);
	MaximumImageRequests->setToolTip(tr("Define limit of images received per minute"));
	connect(MaximumImageRequests, SIGNAL(valueChanged(int)), this, SLOT(dataChanged()));

	imagesLayout->addRow(tr("Limit numbers of image received per minute") + ':', MaximumImageRequests);

	layout->addWidget(images);

	QGroupBox *status = new QGroupBox(tr("Status"), this);
	QFormLayout *statusLayout = new QFormLayout(status);

	PrivateStatus = new QCheckBox(tr("Show my status to everyone"), optionsTab);
	PrivateStatus->setToolTip(tr("When disabled, you're visible only to buddies on your list"));
	connect(PrivateStatus, SIGNAL(clicked()), this, SLOT(dataChanged()));

	statusLayout->addRow(PrivateStatus);

	layout->addWidget(status);
	layout->addStretch(100);
}

void GaduEditAccountWidget::createGeneralGroupBox(QVBoxLayout *layout)
{
	QGroupBox *general = new QGroupBox(tr("General"), this);
	QGridLayout *generalLayout = new QGridLayout(general);
	generalLayout->setColumnMinimumWidth(0, 20);
	generalLayout->setColumnMinimumWidth(3, 20);
	layout->addWidget(general);

	useDefaultServers = new QCheckBox(tr("Use default servers"), this);
	generalLayout->addWidget(useDefaultServers, 0, 0, 1, 4);

	QLabel *ipAddressesLabel = new QLabel(tr("IP addresses"), this);
	ipAddresses = new QLineEdit(this);
	ipAddresses->setToolTip("You can specify which servers and ports to use.\n"
							"Separate every server using semicolon.\n"
							"The last IPv4 octet may be specified as a range of addresses.\n"
							"For example:\n"
							"91.214.237.1 ; 91.214.237.3 ; 91.214.237.10:8074 ; 91.214.237.11-20 ; 91.214.237.21-30:8074");
	generalLayout->addWidget(ipAddressesLabel, 1, 1);
	generalLayout->addWidget(ipAddresses, 1, 2);

	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddressesLabel, SLOT(setDisabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddresses, SLOT(setDisabled(bool)));

	connect(useDefaultServers, SIGNAL(toggled(bool)), this, SLOT(dataChanged()));
	connect(ipAddresses, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
}

void GaduEditAccountWidget::apply()
{
	account().setAccountIdentity(Identities->currentIdentity());
	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());
	account().setPrivateStatus(!PrivateStatus->isChecked());

	if (Details)
	{
		Details->setMaximumImageSize(MaximumImageSize->value());
		Details->setReceiveImagesDuringInvisibility(ReceiveImagesDuringInvisibility->isChecked());
		Details->setMaximumImageRequests(MaximumImageRequests->value());
	}

	Proxy->apply();

	config_file.writeEntry("Network", "isDefServers", useDefaultServers->isChecked());
	config_file.writeEntry("Network", "Server", ipAddresses->text());
	GaduServersManager::instance()->buildServerList();

	if (gpiw->isModified())
		gpiw->apply();

	IdentityManager::instance()->removeUnused();
	ConfigurationManager::instance()->flush();

	resetState();
}

void GaduEditAccountWidget::cancel()
{
	loadAccountData();
	loadConnectionData();
	Proxy->cancel();
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
		&& account().privateStatus() != PrivateStatus->isChecked()
		&& Details->maximumImageSize() == MaximumImageSize->value()
		&& Details->receiveImagesDuringInvisibility() == ReceiveImagesDuringInvisibility->isChecked()
		&& Details->maximumImageRequests() == MaximumImageRequests->value()
		&& config_file.readBoolEntry("Network", "isDefServers", true) == useDefaultServers->isChecked()
		&& config_file.readEntry("Network", "Server") == ipAddresses->text()
		&& StateNotChanged == Proxy->state()
		&& !gpiw->isModified())
	{
		resetState();
		return;
	}

	bool sameIdExists = AccountManager::instance()->byId(account().protocolName(), account().id())
			&& AccountManager::instance()->byId(account().protocolName(), account().id()) != account();

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
	PrivateStatus->setChecked(!account().privateStatus());

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());
	if (details)
	{
		MaximumImageSize->setValue(details->maximumImageSize());
		ReceiveImagesDuringInvisibility->setChecked(details->receiveImagesDuringInvisibility());
		MaximumImageRequests->setValue(details->maximumImageRequests());
	}

	useDefaultServers->setChecked(config_file.readBoolEntry("Network", "isDefServers", true));
	ipAddresses->setText(config_file.readEntry("Network", "Server"));
}

void GaduEditAccountWidget::loadConnectionData()
{
	Proxy->loadProxyData();
}

void GaduEditAccountWidget::removeAccount()
{
	QMessageBox *messageBox = new QMessageBox(this);
	messageBox->setWindowTitle(tr("Confirm account removal"));
	messageBox->setText(tr("Are you sure do you want to remove account %1 (%2)")
			.arg(account().accountIdentity().name())
			.arg(account().id()));

	QPushButton *removeButton = messageBox->addButton(tr("Remove account"), QMessageBox::AcceptRole);
	QPushButton *removeAndUnregisterButton = messageBox->addButton(tr("Remove account and unregister from server"), QMessageBox::DestructiveRole);
	messageBox->addButton(QMessageBox::Cancel);
	messageBox->setDefaultButton(QMessageBox::Cancel);
	messageBox->exec();

	if (messageBox->clickedButton() == removeButton)
	{
		AccountManager::instance()->removeAccountAndBuddies(account());
		deleteLater();
	}
	else if (messageBox->clickedButton() == removeAndUnregisterButton)
		(new GaduUnregisterAccountWindow(account()))->show();

	delete messageBox;
}

void GaduEditAccountWidget::remindPasssword()
{
	bool ok;
	int uin = AccountId->text().toInt(&ok);
	if (ok)
		(new GaduRemindPasswordWindow(uin))->show();
}

void GaduEditAccountWidget::changePasssword()
{
	bool ok;
	int uin = AccountId->text().toInt(&ok);
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
