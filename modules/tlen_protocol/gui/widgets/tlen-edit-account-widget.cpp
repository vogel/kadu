 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
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

	QLabel *numberLabel = new QLabel(tr("Tlen.pl Login") + ":", this);
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

	//QPushButton *changePassword = new QPushButton(tr("Change password"), this);
	//layout->addWidget(changePassword, row++, 1, Qt::AlignLeft);

	QPushButton *removeAccount = new QPushButton(tr("Remove account"), this);
	connect(removeAccount, SIGNAL(clicked(bool)), this, SLOT(removeAccount()));
	layout->addWidget(removeAccount, row++, 1);
	layout->setRowStretch(row, 100);

	row = 0;
	QLabel *photoLabel = new QLabel(tr("Your photo") + ":", this);
	layout->addWidget(photoLabel, row++, 4);

	QPushButton *photoButton = new QPushButton;
	photoButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	layout->addWidget(photoButton, row, 4, 4, 1);

	tabWidget->addTab(generalTab, tr("General"));
}

void TlenEditAccountWidget::createPersonalDataTab(QTabWidget *tabWidget)
{
	gpiw = new TlenPersonalInfoWidget(account(), tabWidget);
	tabWidget->addTab(gpiw, tr("Personal info"));
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

	proxy->applyProxyData();

	gpiw->applyData();
}

void TlenEditAccountWidget::removeAccount()
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
			AccountManager::instance()->unregisterAccount(account());
			deleteLater();
			break;

		case QMessageBox::DestructiveRole:
			// not implemented
			break;
	}

	delete messageBox;
}
