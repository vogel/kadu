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
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "accounts/account.h"

#include "proxy-group-box.h"

ProxyGroupBox::ProxyGroupBox(Account *account, const QString &title, QWidget *parent)
	: QGroupBox(title, parent) ,MyAccount(account)
{
    	QGridLayout *proxyLayout = new QGridLayout(this);
	proxyLayout->setColumnMinimumWidth(0, 20);

	useProxy = new QCheckBox(tr("Use the following proxy"));
	proxyLayout->addWidget(useProxy, 0, 0, 1, 6);
//
	QLabel *hostLabel = new QLabel(tr("Host") + ":");
	hostLabel->setEnabled(false);

	host = new QLineEdit;
	host->setInputMask("000.000.000.000;_");
	host->setEnabled(false);

	proxyLayout->addWidget(hostLabel, 1, 1);
	proxyLayout->addWidget(host, 1, 2);
//
	QLabel *proxyPortLabel = new QLabel(tr("Port") + ":");
	proxyPortLabel->setEnabled(false);

	proxyPort = new QLineEdit;
	proxyPort->setValidator(new QIntValidator(0, 9999999, proxyPort));
	proxyPort->setEnabled(false);

	proxyLayout->addWidget(proxyPortLabel, 1, 4);
	proxyLayout->addWidget(proxyPort, 1, 5);
////
	proxyAuthWidget = new QWidget;
	proxyAuthWidget->setContentsMargins(0, 0, 0, 0);
	proxyAuthWidget->setEnabled(false);

	QGridLayout *authlayout = new QGridLayout(proxyAuthWidget);
	authlayout->setContentsMargins(0, 0, 0, 0);
	authlayout->setColumnMinimumWidth(0, 20);
	authlayout->setColumnStretch(2, 100);

	proxyAuthentication = new QCheckBox(tr("Proxy requires authentication"));
	authlayout->addWidget(proxyAuthentication, 0, 0, 1, 3);
//
	QLabel *usernameLabel = new QLabel(tr("Username") + ":");
	usernameLabel->setEnabled(false);

	username = new QLineEdit;
	username->setEnabled(false);

	authlayout->addWidget(usernameLabel, 1, 1);
	authlayout->addWidget(username, 1, 2, 1, 2);
//
	QLabel *passwordLabel = new QLabel(tr("Password") + ":");
	passwordLabel->setEnabled(false);

	password = new QLineEdit;
	password->setEchoMode(QLineEdit::Password);
	password->setEnabled(false);

	authlayout->addWidget(passwordLabel, 2, 1, Qt::AlignLeft);
	authlayout->addWidget(password, 2, 2, 1, 2);

	proxyLayout->addWidget(proxyAuthWidget, 2, 0, 1, 6);
////
	connect(useProxy, SIGNAL(toggled(bool)), hostLabel, SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), host, SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), proxyPortLabel, SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), proxyPort, SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), proxyAuthWidget, SLOT(setEnabled(bool)));

	connect(proxyAuthentication, SIGNAL(toggled(bool)), usernameLabel, SLOT(setEnabled(bool)));
	connect(proxyAuthentication, SIGNAL(toggled(bool)), username, SLOT(setEnabled(bool)));
	connect(proxyAuthentication, SIGNAL(toggled(bool)), passwordLabel, SLOT(setEnabled(bool)));
	connect(proxyAuthentication, SIGNAL(toggled(bool)), password, SLOT(setEnabled(bool)));
}

void ProxyGroupBox::loadProxyData()
{
    	useProxy->setChecked(MyAccount->useProxy());
	host->setText(MyAccount->proxyHost().toString());
	proxyPort->setText(QString::number(MyAccount->proxyPort()));
    	proxyAuthentication->setChecked(MyAccount->proxyReqAuthentication());
	username->setText(MyAccount->proxyUser());
	password->setText(MyAccount->proxyPassword());
}

void ProxyGroupBox::applyProxyData()
{
    	MyAccount->setUseProxy(useProxy->isChecked());

	QHostAddress hostAdrr;
	if (!hostAdrr.setAddress(host->text()))
		hostAdrr.setAddress("0.0.0.0");
	MyAccount->setProxyHost(hostAdrr);
	MyAccount->setProxyPort(proxyPort->text().toInt());
	MyAccount->setProxyReqAuthentication(proxyAuthentication->isChecked());
	MyAccount->setProxyUser(username->text());
	MyAccount->setProxyPassword(password->text());
}
