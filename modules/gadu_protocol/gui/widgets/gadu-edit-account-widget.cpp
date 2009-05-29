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
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "gui/widgets/gadu-personal-info-widget.h"
#include "gui/widgets/account-contacts-list-widget.h"

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(Account *account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	createGui();
	loadAccountData();
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
// 	tabWidget->addTab(new QWidget(), tr("Connection"));
// 	tabWidget->addTab(new QWidget(), tr("Functions"));
}


void GaduEditAccountWidget::createGeneralTab(QTabWidget *tabWidget)
{
	QWidget *generalTab = new QWidget(this);

	QGridLayout *layout = new QGridLayout(generalTab);
	layout->setColumnMinimumWidth(0, 20);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(5, 2);

	int row = 0;
	ConnectAtStart = new QCheckBox(tr("Connect at start"), this);
	layout->addWidget(ConnectAtStart, row++, 0, 1, 3);

	QLabel *numberLabel = new QLabel(tr("Gadu-Gadu number") + ":", this);
	layout->addWidget(numberLabel, row, 1, Qt::AlignRight);
	AccountId = new QLineEdit(this);
	layout->addWidget(AccountId, row++, 2, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	layout->addWidget(passwordLabel, row, 1, Qt::AlignRight);
	AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addWidget(AccountPassword, row, 2);
	QPushButton *remindPassword = new QPushButton(tr("Forgot password"), this);
	layout->addWidget(remindPassword, row++, 3, Qt::AlignLeft);

	QPushButton *changePassword = new QPushButton(tr("Change password"), this);
	layout->addWidget(changePassword, row++, 3, Qt::AlignLeft);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	layout->addWidget(RememberPassword, row++, 2, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	layout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	QComboBox *description = new QComboBox(this);
	layout->addWidget(description, row++, 2, 1, 2);

	layout->setRowMinimumHeight(row++, 60);

	QPushButton *removeAccount = new QPushButton(tr("Remove account"), this);
	connect(removeAccount, SIGNAL(clicked(bool)), this, SLOT(removeAccount()));
	layout->addWidget(removeAccount, row++, 1, 1, 3);
	layout->setRowStretch(row, 100);

	row = 0;
	QLabel *photoLabel = new QLabel(tr("Your picture"), this);
	layout->addWidget(photoLabel, row++, 5);

	QFrame *frame = new QFrame(this);
	frame->setFrameShape(QFrame::Box);
	layout->addWidget(frame, row, 5, 1, 4);
	row += 4;

	QPushButton *changePicture = new QPushButton(tr("Change picture"), this);
	layout->addWidget(changePicture, row++, 5);

	tabWidget->addTab(generalTab, tr("General"));
}

void GaduEditAccountWidget::createPersonalDataTab(QTabWidget *tabWidget)
{
	GaduPersonalInfoWidget *gpiw = new GaduPersonalInfoWidget(account(), tabWidget);
	tabWidget->addTab(gpiw, tr("Personal info"));
}

void GaduEditAccountWidget::createBuddiesTab(QTabWidget *tabWidget)
{
	AccountContactsListWidget *widget = new AccountContactsListWidget(account(), this);
	tabWidget->addTab(widget, tr("Buddies"));
}


void GaduEditAccountWidget::loadAccountData()
{
	ConnectAtStart->setChecked(account()->connectAtStart());
	AccountId->setText(account()->id());
	RememberPassword->setChecked(account()->rememberPsssword());
	AccountPassword->setText(account()->password());
}

void GaduEditAccountWidget::apply()
{
	account()->setConnectAtStart(ConnectAtStart->isChecked());
	account()->setId(AccountId->text());
	account()->setRememberPassword(RememberPassword->isChecked());
	account()->setPassword(AccountPassword->text());
}

void GaduEditAccountWidget::removeAccount()
{
	QMessageBox *messageBox = new QMessageBox(this);
	messageBox->setWindowTitle(tr("Confirm account removal"));
	messageBox->setText(tr("Are you sure do you want to remove account %1 (%2)")
			.arg(account()->name())
			.arg(account()->id()));

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
