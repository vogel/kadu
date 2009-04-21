 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(QWidget *parent) :
		QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

JabberCreateAccountWidget::~JabberCreateAccountWidget()
{
}

void JabberCreateAccountWidget::createGui()
{
	/*
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);

	gridLayout->setColumnStretch(1, 100);

	QRadioButton *haveNumber = new QRadioButton(tr("I have a Gadu-Gadu number"), this);
	connect(haveNumber, SIGNAL(toggled(bool)), this, SLOT(haveNumberChanged(bool)));
	gridLayout->addWidget(haveNumber, 0, 0, 1, 2);

	QLabel *ggNumberLabel = new QLabel(tr("Gadu-Gadu number") + ":", this);
	gridLayout->addWidget(ggNumberLabel, 1, 0, Qt::AlignRight);
	QLineEdit *ggNumber = new QLineEdit(this);
	gridLayout->addWidget(ggNumber, 1, 1);

	QLabel *ggPasswordLabel = new QLabel(tr("Password") + ":", this);
	gridLayout->addWidget(ggPasswordLabel, 2, 0, Qt::AlignRight);
	QLineEdit *ggPassword = new QLineEdit(this);
	ggPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggPassword, 2, 1);

	QCheckBox *ggImportContacts = new QCheckBox(tr("Import contacts"), this);
	ggImportContacts->setChecked(true);
	gridLayout->addWidget(ggImportContacts, 3, 1, 1, 2);

	QRadioButton *dontHaveNumber = new QRadioButton(tr("I don't have a Gadu-Gadu number"), this);
	gridLayout->addWidget(dontHaveNumber, 4, 0, 1, 2);

	QLabel *ggNewPasswordLabel = new QLabel(tr("New password") + ":", this);
	gridLayout->addWidget(ggNewPasswordLabel, 5, 0, Qt::AlignRight);
	QLineEdit *ggNewPassword = new QLineEdit(this);
	ggNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggNewPassword, 5, 1);

	QLabel *ggReNewPasswordLabel = new QLabel(tr("Retype password") + ":", this);
	gridLayout->addWidget(ggReNewPasswordLabel, 6, 0, Qt::AlignRight);
	QLineEdit *ggReNewPassword = new QLineEdit(this);
	ggReNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(ggReNewPassword, 6, 1);

	QLabel *ggEMailLabel = new QLabel(tr("Your e-mail address") + ":", this);
	gridLayout->addWidget(ggEMailLabel, 7, 0, Qt::AlignRight);
	QLineEdit *ggEMail = new QLineEdit(this);
	gridLayout->addWidget(ggEMail, 7, 1);

	QPushButton *ggRegisterAccount = new QPushButton(tr("Register"), this);
	gridLayout->addWidget(ggRegisterAccount, 8, 0, 1, 2);

	HaveNumberWidgets.append(ggNumberLabel);
	HaveNumberWidgets.append(ggNumber);
	HaveNumberWidgets.append(ggPasswordLabel);
	HaveNumberWidgets.append(ggPassword);
	HaveNumberWidgets.append(ggImportContacts);
	DontHaveNumberWidgets.append(ggNewPasswordLabel);
	DontHaveNumberWidgets.append(ggNewPassword);
	DontHaveNumberWidgets.append(ggReNewPasswordLabel);
	DontHaveNumberWidgets.append(ggReNewPassword);
	DontHaveNumberWidgets.append(ggEMailLabel);
	DontHaveNumberWidgets.append(ggEMail);
	DontHaveNumberWidgets.append(ggRegisterAccount);

	haveNumberChanged(true);
	*/
}

void JabberCreateAccountWidget::haveNumberChanged(bool haveNumber)
{
	foreach (QWidget *widget, HaveNumberWidgets)
		widget->setVisible(haveNumber);
	foreach (QWidget *widget, DontHaveNumberWidgets)
		widget->setVisible(!haveNumber);
}
