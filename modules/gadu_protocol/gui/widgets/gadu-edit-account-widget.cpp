/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QFileDialog>
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
#include "gui/widgets/choose-identity-widget.h"
#include "gui/widgets/proxy-group-box.h"
#include "protocols/services/avatar-service.h"
#include "protocols/services/contact-list-service.h"
#include "protocols/protocol.h"

#include "services/gadu-contact-list-service.h"

#include "gadu-personal-info-widget.h"

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(Account account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	createGui();
	loadAccountData();
	loadConnectionData();
}

GaduEditAccountWidget::~GaduEditAccountWidget()
{
}

void GaduEditAccountWidget::createGui()
{
	QGridLayout *mainLayout = new QGridLayout(this);

	QTabWidget *tabWidget = new QTabWidget(this);
	mainLayout->addWidget(tabWidget);

	createGeneralTab(tabWidget);
	createPersonalDataTab(tabWidget);
	createBuddiesTab(tabWidget);
	createConnectionTab(tabWidget);
// 	tabWidget->addTab(new QWidget(), tr("Functions"));
}


void GaduEditAccountWidget::createGeneralTab(QTabWidget *tabWidget)
{
	QWidget *generalTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(generalTab);
	layout->setColumnMinimumWidth(0, 20);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnMinimumWidth(5, 100);
	layout->setColumnMinimumWidth(6, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(6, 2);

	int row = 0;
	ConnectAtStart = new QCheckBox(tr("Connect at start"), this);
	layout->addWidget(ConnectAtStart, row++, 0, 1, 3);

	QLabel *numberLabel = new QLabel(tr("Gadu-Gadu number") + ":", this);
	layout->addWidget(numberLabel, row++, 1);
	AccountId = new QLineEdit(this);
	layout->addWidget(AccountId, row++, 1, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	layout->addWidget(passwordLabel, row++, 1);
	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addWidget(AccountPassword, row++, 1, 1, 2);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	layout->addWidget(RememberPassword, row++, 1, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description") + ":", this);
	layout->addWidget(descriptionLabel, row++, 1, Qt::AlignRight);
	ChooseIdentity = new ChooseIdentityWidget(this);
	layout->addWidget(ChooseIdentity, row++, 1, 1, 2);

	layout->setRowMinimumHeight(row++, 30);

	QPushButton *remindPassword = new QPushButton(tr("Forgot password"), this);
	layout->addWidget(remindPassword, row, 1, Qt::AlignLeft);

	QPushButton *changePassword = new QPushButton(tr("Change password"), this);
	layout->addWidget(changePassword, row++, 2, Qt::AlignLeft);

	QPushButton *removeAccount = new QPushButton(tr("Remove account"), this);
	connect(removeAccount, SIGNAL(clicked(bool)), this, SLOT(removeAccount()));
	layout->addWidget(removeAccount, row++, 1, 1, 3);
	layout->setRowStretch(row, 100);

	row = 0;
	AccountAvatarWidget *avatarWidget = new AccountAvatarWidget(account(), this);
	layout->addWidget(avatarWidget, row++, 5, 5, 1);

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

	proxy = new ProxyGroupBox(account(), tr("Proxy"), this);
	layout->addWidget(proxy);

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

	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddressesLabel, SLOT(setEnabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), ipAddresses, SLOT(setEnabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), portLabel, SLOT(setEnabled(bool)));
	connect(useDefaultServers, SIGNAL(toggled(bool)), port, SLOT(setEnabled(bool)));
}

void GaduEditAccountWidget::loadAccountData()
{
	ConnectAtStart->setChecked(account().connectAtStart());
	AccountId->setText(account().id());
	RememberPassword->setChecked(account().rememberPassword());
	AccountPassword->setText(account().password());
}

void GaduEditAccountWidget::loadConnectionData()
{
	proxy->loadProxyData();
}

void GaduEditAccountWidget::apply()
{
	account().setConnectAtStart(ConnectAtStart->isChecked());
	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());

	proxy->applyProxyData();

	gpiw->applyData();
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
