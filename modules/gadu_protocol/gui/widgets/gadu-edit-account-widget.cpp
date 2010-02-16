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
#include <QtGui/QTabWidget>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/account-avatar-widget.h"
#include "gui/widgets/account-buddy-list-widget.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/widgets/proxy-group-box.h"
#include "protocols/services/avatar-service.h"
#include "protocols/services/contact-list-service.h"
#include "protocols/protocol.h"
#include "icons-manager.h"

#include "gui/windows/gadu-remind-password-window.h"
#include "services/gadu-contact-list-service.h"

#include "gadu-personal-info-widget.h"

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(Account account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	createGui();
	loadAccountData();
	loadConnectionData();

	dataChanged();
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
	createPersonalDataTab(tabWidget);
	createBuddiesTab(tabWidget);
	createConnectionTab(tabWidget);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	ApplyButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-apply.png"), tr("Apply"), this);
	connect(ApplyButton, SIGNAL(clicked(bool)), this, SLOT(apply()));

	CancelButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-cancel.png"), tr("Cancel"), this);
	connect(CancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	QPushButton *removeAccount = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-cancel.png"), tr("Delete account"), this);
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

	ConnectAtStart = new QCheckBox(tr("Connect at start"), this);
	connect(ConnectAtStart, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));
	formLayout->addRow(0, ConnectAtStart);

	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Account name") + ":", AccountName);

	AccountId = new QLineEdit(this);
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Gadu-Gadu number") + ":", AccountId);

	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Password") + ":", AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	connect(RememberPassword, SIGNAL(stateChanged(int)), this, SLOT(dataChanged()));
	formLayout->addRow(0, RememberPassword);

	QLabel *remindPasswordButton = new QLabel(QString("<a href='remind'>%1</a>").arg(tr("Forgot Your Password?")));
	remindPasswordButton->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	formLayout->addRow(0, remindPasswordButton);
	connect(remindPasswordButton, SIGNAL(linkActivated(QString)), this, SLOT(remindPasssword()));

	Identities = new IdentitiesComboBox(this);
	connect(Identities, SIGNAL(activated(int)), this, SLOT(dataChanged()));
	formLayout->addRow(tr("Account Identity") + ":", Identities);

	formLayout->addRow(0, new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this));

	AccountAvatarWidget *avatarWidget = new AccountAvatarWidget(account(), this);
	layout->addWidget(avatarWidget, 0, 1, Qt::AlignTop);

	tabWidget->addTab(generalTab, tr("General"));
}

void GaduEditAccountWidget::createPersonalDataTab(QTabWidget *tabWidget)
{
	gpiw = new GaduPersonalInfoWidget(account(), tabWidget);
	tabWidget->addTab(gpiw, tr("Personal info"));
}

void GaduEditAccountWidget::createBuddiesTab(QTabWidget *tabWidget)
{
	QWidget *widget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(widget);

	AccountBuddyListWidget *buddiesWidget = new AccountBuddyListWidget(account(), widget);
	layout->addWidget(buddiesWidget);

	QPushButton *getListAsFile = new QPushButton("Import contacts list as file", widget);
	connect(getListAsFile, SIGNAL(clicked(bool)), this, SLOT(importListAsFile()));
	layout->addWidget(getListAsFile);

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

void GaduEditAccountWidget::createGeneralGroupBox(QVBoxLayout *layout)
{
	QGroupBox *general = new QGroupBox(tr("General"), this);
	QGridLayout *generalLayout = new QGridLayout(general);
	generalLayout->setColumnMinimumWidth(0, 20);
	generalLayout->setColumnMinimumWidth(3, 20);
	layout->addWidget(general);

	useDefaultServers = new QCheckBox(tr("Use default servers"), this);
	generalLayout->addWidget(useDefaultServers, 0, 0, 1, 6);

	QLabel *ipAddressesLabel = new QLabel(tr("IP addresses"), this);
	ipAddresses = new QLineEdit(this);
	ipAddresses->setToolTip("You can specify which GG servers to use. Separate every server using semicolon\n"
				"(for example: 91.197.13.26;91.197.13.24;91.197.13.29;91.197.13.6)");
	generalLayout->addWidget(ipAddressesLabel, 1, 1);
	generalLayout->addWidget(ipAddresses, 1, 2);

	QLabel *portLabel = new QLabel(tr("Port"), this);
	port = new QComboBox(this);
	port->addItem("Automatic");
	port->addItem("8074");
	port->addItem("443");
	generalLayout->addWidget(portLabel, 1, 4);
	generalLayout->addWidget(port, 1, 5);

	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddressesLabel, SLOT(setDisabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddresses, SLOT(setDisabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), portLabel, SLOT(setDisabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), port, SLOT(setDisabled(bool)));
}

void GaduEditAccountWidget::apply()
{
	account().setName(AccountName->text());
	account().setConnectAtStart(ConnectAtStart->isChecked());
	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());

	Proxy->apply();

	gpiw->applyData();

	setState(StateNotChanged);
}

void GaduEditAccountWidget::cancel()
{

}

// TODO: 0.6.6 check proxy data too
void GaduEditAccountWidget::dataChanged()
{
	if (account().name() == AccountName->text()
		&& account().connectAtStart() == ConnectAtStart->isChecked()
		&& account().id() == AccountId->text()
		&& account().rememberPassword() == RememberPassword->isChecked()
		&& account().password() == AccountPassword->text()
		&& StateNotChanged == Proxy->state())
	{
		setState(StateNotChanged);
		ApplyButton->setEnabled(false);
		CancelButton->setEnabled(false);
		return;
	}

	bool sameNameExists = AccountManager::instance()->byName(AccountName->text())
			&& AccountManager::instance()->byName(AccountName->text()) != account();
	bool sameIdExists = AccountManager::instance()->byId(account().protocolName(), account().id())
			&& AccountManager::instance()->byId(account().protocolName(), account().id()) != account();

	if (AccountName->text().isEmpty()
		|| sameNameExists
		|| AccountId->text().isEmpty()
		|| sameIdExists)
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
	AccountName->setText(account().name());
	ConnectAtStart->setChecked(account().connectAtStart());
	AccountId->setText(account().id());
	RememberPassword->setChecked(account().rememberPassword());
	AccountPassword->setText(account().password());
	// Identities->setCurrentIdentity(account().identity());
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
			.arg(account().name())
			.arg(account().id()));

	messageBox->addButton(tr("Remove account"), QMessageBox::AcceptRole);
	messageBox->addButton(tr("Remove account and unregister from server"), QMessageBox::DestructiveRole);
	messageBox->addButton(QMessageBox::Cancel);

	switch (messageBox->exec())
	{
		case QMessageBox::AcceptRole:
			AccountManager::instance()->removeItem(account());
			deleteLater();
			break;

		case QMessageBox::DestructiveRole:
			// TODO: 0.6.6, implement unregister here
			AccountManager::instance()->removeItem(account());
			deleteLater();
			break;
	}

	delete messageBox;
}

void GaduEditAccountWidget::importListAsFile()
{
	Protocol *protocol = account().protocolHandler();
	if (!protocol)
		return;

	ContactListService *service = protocol->contactListService();
	if (!service)
		return;

	GaduContactListService *gaduService = dynamic_cast<GaduContactListService *>(service);
	if (!gaduService)
		return;

	connect(gaduService, SIGNAL(contactListDownloaded(QString)), this, SLOT(contactListDownloaded(QString)));
	gaduService->importContactListAsFile();
}

void GaduEditAccountWidget::contactListDownloaded(QString content)
{
	QString fileName = QFileDialog::getSaveFileName();
	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);
	file.write(content.toLocal8Bit());
	file.close();
}


void GaduEditAccountWidget::remindPasssword()
{
	bool ok;
	int uin = AccountId->text().toInt(&ok);
	if (ok)
		(new GaduRemindPasswordWindow(uin))->show();
}
