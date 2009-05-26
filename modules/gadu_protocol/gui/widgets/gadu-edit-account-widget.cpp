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
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(Account *account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}


GaduEditAccountWidget::~GaduEditAccountWidget()
{
}

void GaduEditAccountWidget::createGui()
{
	QTabWidget *tabWidget = new QTabWidget(this);

	createGeneralTab(tabWidget);
	tabWidget->addTab(new QWidget(), tr("Personal data"));
	tabWidget->addTab(new QWidget(), tr("Buddies"));
	tabWidget->addTab(new QWidget(), tr("Connection"));
	tabWidget->addTab(new QWidget(), tr("Functions"));
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
	QCheckBox *connectAtStart = new QCheckBox(tr("Connect at start"), this);
	layout->addWidget(connectAtStart, row++, 0, 1, 3);

	QLabel *numberLabel = new QLabel(tr("Gadu-Gadu number") + ":", this);
	layout->addWidget(numberLabel, row, 1, Qt::AlignRight);
	QLineEdit *AccountId = new QLineEdit(this);
	layout->addWidget(AccountId, row++, 2, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	layout->addWidget(passwordLabel, row, 1, Qt::AlignRight);
	QLineEdit *AccountPassword = new QLineEdit(this);
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addWidget(AccountPassword, row, 2);
	QPushButton *remindPassword = new QPushButton(tr("Forgot password"), this);
	layout->addWidget(remindPassword, row++, 3, Qt::AlignLeft);

	QPushButton *changePassword = new QPushButton(tr("Change password"), this);
	layout->addWidget(changePassword, row++, 3, Qt::AlignLeft);

	QCheckBox *rememberPassword = new QCheckBox(tr("Remember password"), this);
	rememberPassword->setChecked(true);
	layout->addWidget(rememberPassword, row++, 2, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	layout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	QComboBox *description = new QComboBox(this);
	layout->addWidget(description, row++, 2, 1, 2);

	layout->setRowMinimumHeight(row++, 60);

	QPushButton *removeAccount = new QPushButton(tr("Remove account"), this);
	layout->addWidget(removeAccount, row++, 1, 1, 3);

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
