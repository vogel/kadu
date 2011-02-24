/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "gui/widgets/account-buddy-list-widget.h"
#include "gui/widgets/choose-identity-widget.h"

#include "gui/widgets/proxy-group-box.h"

#include "tlen-personal-info-widget.h"

#include "tlen-edit-account-widget.h"

TlenEditAccountWidget::TlenEditAccountWidget(Account account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	createGui();

	loadAccountData();
	loadConnectionData();
}

TlenEditAccountWidget::~TlenEditAccountWidget()
{
}

void TlenEditAccountWidget::createGui()
{
	QGridLayout *mainLayout = new QGridLayout(this);

	QTabWidget *tabWidget = new QTabWidget(this);
	mainLayout->addWidget(tabWidget);

	createGeneralTab(tabWidget);
	createPersonalDataTab(tabWidget);
	createBuddiesTab(tabWidget);
	createConnectionTab(tabWidget);
// 	tabWidget->addTab(new QWidget(), tr("Functions"));

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


void TlenEditAccountWidget::createGeneralTab(QTabWidget *tabWidget)
{
	QWidget *generalTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(generalTab);
	layout->setColumnMinimumWidth(0, 20);
	layout->setColumnMinimumWidth(3, 20);
	layout->setColumnMinimumWidth(4, 100);
	layout->setColumnMinimumWidth(5, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(5, 2);

	int row = 0;
	ConnectAtStart = new QCheckBox(tr("Connect at start"), this);
	layout->addWidget(ConnectAtStart, row++, 0, 1, 3);

	QLabel *nameLabel = new QLabel(tr("Account name") + ':', this);
	layout->addWidget(nameLabel, row++, 1);
	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addWidget(AccountName, row++, 1, 1, 2);

	QLabel *numberLabel = new QLabel(tr("Tlen.pl Login") + ':', this);
	layout->addWidget(numberLabel, row++, 1);
	AccountId = new QLineEdit(this);
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addWidget(AccountId, row++, 1, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ':', this);
	layout->addWidget(passwordLabel, row++, 1);
	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addWidget(AccountPassword, row++, 1, 1, 2);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	layout->addWidget(RememberPassword, row++, 1, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description") + ':', this);
	layout->addWidget(descriptionLabel, row++, 1, Qt::AlignRight);
	ChooseIdentity = new ChooseIdentityWidget(this);
	layout->addWidget(ChooseIdentity, row++, 1, 1, 2);

	layout->setRowMinimumHeight(row++, 30);

	//QPushButton *changePassword = new QPushButton(tr("Change password"), this);
	//layout->addWidget(changePassword, row++, 1, Qt::AlignLeft);

	row = 0;
	QLabel *photoLabel = new QLabel(tr("Your photo") + ':', this);
	layout->addWidget(photoLabel, row++, 4);

	QPushButton *photoButton = new QPushButton;
	photoButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	layout->addWidget(photoButton, row, 4, 4, 1);

	tabWidget->addTab(generalTab, tr("General"));
}

void TlenEditAccountWidget::createPersonalDataTab(QTabWidget *tabWidget)
{
	gpiw = new TlenPersonalInfoWidget(account(), tabWidget);
	tabWidget->addTab(gpiw, tr("Personal Information"));
}

void TlenEditAccountWidget::createBuddiesTab(QTabWidget *tabWidget)
{
	AccountBuddyListWidget *widget = new AccountBuddyListWidget(account(), this);
	tabWidget->addTab(widget, tr("Buddies"));
}

void TlenEditAccountWidget::createConnectionTab(QTabWidget *tabWidget)
{
	QWidget *conenctionTab = new QWidget(this);
	tabWidget->addTab(conenctionTab, tr("Connection"));

	QVBoxLayout *layout = new QVBoxLayout(conenctionTab);

	proxy = new ProxyGroupBox(account(), tr("Proxy"), this);
	layout->addWidget(proxy);

	layout->addStretch(100);
}

void TlenEditAccountWidget::loadAccountData()
{
	ConnectAtStart->setChecked(account().connectAtStart());
	AccountId->setText(account().id());
	RememberPassword->setChecked(account().rememberPassword());
	AccountPassword->setText(account().password());
}

void TlenEditAccountWidget::loadConnectionData()
{
	proxy->loadProxyData();
}

void TlenEditAccountWidget::apply()
{
	account().setConnectAtStart(ConnectAtStart->isChecked());
	account().setId(AccountId->text());
	account().setRememberPassword(RememberPassword->isChecked());
	account().setPassword(AccountPassword->text());
	account().setHasPassword(!AccountPassword->text().isEmpty());

	proxy->apply();

	gpiw->applyData();

	ConfigurationManager::instance()->flush();

	setState(StateNotChanged);
}

void TlenEditAccountWidget::cancel()
{
}

void TlenEditAccountWidget::removeAccount()
{
	QMessageBox *messageBox = new QMessageBox(this);
	messageBox->setWindowTitle(tr("Confirm account removal"));
	messageBox->setText(tr("Are you sure do you want to remove account %1 (%2)")
			.arg(account().accountIdentity().name())
			.arg(account().id()));

	QPushButton *removeButton = messageBox->addButton(tr("Remove Account"), QMessageBox::AcceptRole);
	//QPushButton *removeAndUnregisterButton = messageBox->addButton(tr("Remove account and unregister from server"), QMessageBox::DestructiveRole);
	messageBox->addButton(QMessageBox::Cancel);
	messageBox->setDefaultButton(QMessageBox::Cancel);
	messageBox->exec();

	if (messageBox->clickedButton() == removeButton)
	{
		AccountManager::instance()->removeItem(account());
		deleteLater();
	}
// 	else if (messageBox->clickedButton() == removeAndUnregisterButton)
// 	{
// 		AccountManager::instance()->removeItem(account());
// 		deleteLater();
// 	}

	delete messageBox;
}
