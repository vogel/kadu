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
		 AccountCreateWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

JabberCreateAccountWidget::~JabberCreateAccountWidget()
{
}

void JabberCreateAccountWidget::createGui()
{
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);

	gridLayout->setColumnStretch(1, 100);

	QRadioButton *haveJid = new QRadioButton(tr("I have a XMPP/Jabber account"), this);
	connect(haveJid, SIGNAL(tojabberled(bool)), this, SLOT(haveJidChanged(bool)));
	gridLayout->addWidget(haveJid, 0, 0, 1, 2);

	QLabel *jabberJidLabel = new QLabel(tr("Jabber ID") + ":", this);
	gridLayout->addWidget(jabberJidLabel, 1, 0, Qt::AlignRight);
	QLineEdit *jabberJid = new QLineEdit(this);
	gridLayout->addWidget(jabberJid, 1, 1);

	QLabel *jabberPasswordLabel = new QLabel(tr("Password") + ":", this);
	gridLayout->addWidget(jabberPasswordLabel, 2, 0, Qt::AlignRight);
	QLineEdit *jabberPassword = new QLineEdit(this);
	jabberPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(jabberPassword, 2, 1);

	QRadioButton *dontHaveJid = new QRadioButton(tr("I don't have a XMPP/Jabber account"), this);
	gridLayout->addWidget(dontHaveJid, 4, 0, 1, 2);

	QLabel *jabberEMailLabel = new QLabel(tr("Jabber ID") + ":", this);
	gridLayout->addWidget(jabberEMailLabel, 5, 0, Qt::AlignRight);
	QLineEdit *jabberEMail = new QLineEdit(this);
	gridLayout->addWidget(jabberEMail, 5, 1);

	QLabel *jabberNewPasswordLabel = new QLabel(tr("New password") + ":", this);
	gridLayout->addWidget(jabberNewPasswordLabel, 6, 0, Qt::AlignRight);
	QLineEdit *jabberNewPassword = new QLineEdit(this);
	jabberNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(jabberNewPassword, 6, 1);

	QLabel *jabberReNewPasswordLabel = new QLabel(tr("Retype password") + ":", this);
	gridLayout->addWidget(jabberReNewPasswordLabel, 7, 0, Qt::AlignRight);
	QLineEdit *jabberReNewPassword = new QLineEdit(this);
	jabberReNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(jabberReNewPassword, 7, 1);

	QPushButton *jabberRegisterAccount = new QPushButton(tr("Register"), this);
	gridLayout->addWidget(jabberRegisterAccount, 8, 0, 1, 2);

	HaveJidWidgets.append(jabberJidLabel);
	HaveJidWidgets.append(jabberJid);
	HaveJidWidgets.append(jabberPasswordLabel);
	HaveJidWidgets.append(jabberPassword);
	DontHaveJidWidgets.append(jabberNewPasswordLabel);
	DontHaveJidWidgets.append(jabberNewPassword);
	DontHaveJidWidgets.append(jabberReNewPasswordLabel);
	DontHaveJidWidgets.append(jabberReNewPassword);
	DontHaveJidWidgets.append(jabberEMailLabel);
	DontHaveJidWidgets.append(jabberEMail);
	DontHaveJidWidgets.append(jabberRegisterAccount);

	haveJidChanged(true);
}

void JabberCreateAccountWidget::haveJidChanged(bool haveJid)
{
	foreach (QWidget *widget, HaveJidWidgets)
		widget->setVisible(haveJid);
	foreach (QWidget *widget, DontHaveJidWidgets)
		widget->setVisible(!haveJid);
}
