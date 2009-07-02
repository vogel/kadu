 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CREATE_ACCOUNT_WIDGET_H
#define JABBER_CREATE_ACCOUNT_WIDGET_H

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include "gui/widgets/account-create-widget.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

class JabberServerRegisterAccount;
class TokenWidget;

class JabberCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QList<QWidget *> HaveJidWidgets;
	QList<QWidget *> DontHaveJidWidgets;

	QLineEdit *AccountName;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;

	QPushButton *AddThisAccount;
	QPushButton *RemindPassword;

	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	QLineEdit *Server;
	QLineEdit *Username;
	QPushButton *RegisterAccount;
	QGroupBox *ConnectionOptions;
	//TODO zmiana nazewnictwa po chamskim przeszczepie
	QCheckBox *ck_host;
	QHBoxLayout *hboxLayout;
	QLabel *lb_host;
	QLineEdit *le_host;
	QLabel *lb_port;
	QLineEdit *le_port;
	QLabel *lb_ssl;
	QComboBox *cb_ssl;
	QCheckBox *ck_legacy_ssl_probe;

	void createGui();
	void createIHaveAccountGui(QGridLayout *gridLayout, int &row);
	void createRegisterAccountGui(QGridLayout *gridLayout, int &row);

private slots:
	void haveJidChanged(bool haveNumber);
	void iHaveAccountDataChanged();
	void addThisAccount();
	void registerAccountDataChanged();
	void registerNewAccount();
	void registerNewAccountFinished(JabberServerRegisterAccount *jsra);

public:
	explicit JabberCreateAccountWidget(QWidget *parent = 0);
	virtual ~JabberCreateAccountWidget();

};

#endif // JABBER_CREATE_ACCOUNT_WIDGET_H
