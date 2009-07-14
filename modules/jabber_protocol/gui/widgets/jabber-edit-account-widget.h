 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_EDIT_ACCOUNT_WIDGET_H
#define JABBER_EDIT_ACCOUNT_WIDGET_H

#include "gui/widgets/account-edit-widget.h"

class QCheckBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;

class ProxyGroupBox;

class JabberEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	QCheckBox *ConnectAtStart;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	QLineEdit *newAccountDescription;

	ProxyGroupBox *proxy;

	void createGui();
	void createGeneralTab(QTabWidget *);
	void createPersonalDataTab(QTabWidget *);
	void createBuddiesTab(QTabWidget *);
	void createConnectionTab(QTabWidget *);
	void createGeneralGroupBox(QVBoxLayout *layout);

	void loadAccountData();
	void loadConnectionData();

private slots:
	void removeAccount();

public:
	explicit JabberEditAccountWidget(Account *account, QWidget *parent = 0);
	virtual ~JabberEditAccountWidget();

	virtual void apply();

};

#endif // JABBER_EDIT_ACCOUNT_WIDGET_H
