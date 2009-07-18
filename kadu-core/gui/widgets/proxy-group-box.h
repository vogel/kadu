/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROXY_GROUP_BOX_H
#define PROXY_GROUP_BOX_H

#include <QtGui/QGroupBox>

#include "exports.h"

class Account;

class QCheckBox;
class QLabel;
class QLineEdit;

class KADUAPI ProxyGroupBox : public QGroupBox
{
	Account *MyAccount;

	QWidget *proxyAuthWidget;

	QCheckBox *useProxy;
	QLineEdit *host;
	QLineEdit *proxyPort;
	QCheckBox *proxyAuthentication;
	QLineEdit *username;
	QLineEdit *password;

public:
	ProxyGroupBox(Account *account, const QString &title, QWidget *parent = 0);

	void loadProxyData();
	void applyProxyData();
};

#endif // PROXY_GROUP_BOX_H
