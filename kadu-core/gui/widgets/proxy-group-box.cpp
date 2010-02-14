/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "accounts/account.h"

#include "proxy-group-box.h"

ProxyGroupBox::ProxyGroupBox(Account account, const QString &title, QWidget *parent) :
		ModalConfigurationWidget(parent), MyAccount(account)
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	GroupBox = new QGroupBox(title, this);
	layout->addWidget(GroupBox);

	QGridLayout *proxyLayout = new QGridLayout(GroupBox);
	proxyLayout->setColumnMinimumWidth(0, 20);

	UseProxy = new QCheckBox(tr("Use the following proxy"));
	proxyLayout->addWidget(UseProxy, 0, 0, 1, 6);

	QLabel *hostLabel = new QLabel(tr("Host") + ":");
	hostLabel->setEnabled(false);

	Host = new QLineEdit(this);
	Host->setInputMask("000.000.000.000;_");
	Host->setEnabled(false);

	proxyLayout->addWidget(hostLabel, 1, 1);
	proxyLayout->addWidget(Host, 1, 2);

	QLabel *proxyPortLabel = new QLabel(tr("Port") + ":");
	proxyPortLabel->setEnabled(false);

	ProxyPort = new QLineEdit(this);
	ProxyPort->setValidator(new QIntValidator(0, 99999, ProxyPort));
	ProxyPort->setEnabled(false);

	proxyLayout->addWidget(proxyPortLabel, 1, 4);
	proxyLayout->addWidget(ProxyPort, 1, 5);

	ProxyAuthWidget = new QWidget(this);
	ProxyAuthWidget->setContentsMargins(0, 0, 0, 0);
	ProxyAuthWidget->setEnabled(false);

	QGridLayout *authlayout = new QGridLayout(ProxyAuthWidget);
	authlayout->setContentsMargins(0, 0, 0, 0);
	authlayout->setColumnMinimumWidth(0, 20);
	authlayout->setColumnStretch(2, 100);

	ProxyAuthentication = new QCheckBox(tr("Proxy requires authentication"));
	authlayout->addWidget(ProxyAuthentication, 0, 0, 1, 3);

	QLabel *usernameLabel = new QLabel(tr("Username") + ":");
	usernameLabel->setEnabled(false);

	Username = new QLineEdit(this);
	Username->setEnabled(false);

	authlayout->addWidget(usernameLabel, 1, 1);
	authlayout->addWidget(Username, 1, 2, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ":");
	passwordLabel->setEnabled(false);

	Password = new QLineEdit(this);
	Password->setEchoMode(QLineEdit::Password);
	Password->setEnabled(false);

	authlayout->addWidget(passwordLabel, 2, 1, Qt::AlignLeft);
	authlayout->addWidget(Password, 2, 2, 1, 2);

	proxyLayout->addWidget(ProxyAuthWidget, 2, 0, 1, 6);

	connect(UseProxy, SIGNAL(toggled(bool)), hostLabel, SLOT(setEnabled(bool)));
	connect(UseProxy, SIGNAL(toggled(bool)), Host, SLOT(setEnabled(bool)));
	connect(UseProxy, SIGNAL(toggled(bool)), proxyPortLabel, SLOT(setEnabled(bool)));
	connect(UseProxy, SIGNAL(toggled(bool)), ProxyPort, SLOT(setEnabled(bool)));
	connect(UseProxy, SIGNAL(toggled(bool)), ProxyAuthWidget, SLOT(setEnabled(bool)));

	connect(ProxyAuthentication, SIGNAL(toggled(bool)), usernameLabel, SLOT(setEnabled(bool)));
	connect(ProxyAuthentication, SIGNAL(toggled(bool)), Username, SLOT(setEnabled(bool)));
	connect(ProxyAuthentication, SIGNAL(toggled(bool)), passwordLabel, SLOT(setEnabled(bool)));
	connect(ProxyAuthentication, SIGNAL(toggled(bool)), Password, SLOT(setEnabled(bool)));
}

void ProxyGroupBox::loadProxyData()
{
	UseProxy->setChecked(MyAccount.useProxy());
	Host->setText(MyAccount.proxyHost().toString());
	ProxyPort->setText(QString::number(MyAccount.proxyPort()));
	ProxyAuthentication->setChecked(MyAccount.proxyRequiresAuthentication());
	Username->setText(MyAccount.proxyUser());
	Password->setText(MyAccount.proxyPassword());
}

void ProxyGroupBox::apply()
{
	MyAccount.setUseProxy(UseProxy->isChecked());

	QHostAddress hostAdrr;
	if (!hostAdrr.setAddress(Host->text()))
		hostAdrr.setAddress("0.0.0.0");
	MyAccount.setProxyHost(hostAdrr);
	MyAccount.setProxyPort(ProxyPort->text().toInt());
	MyAccount.setProxyRequiresAuthentication(ProxyAuthentication->isChecked());
	MyAccount.setProxyUser(Username->text());
	MyAccount.setProxyPassword(Password->text());
}

void ProxyGroupBox::cancel()
{
	loadProxyData();
}
