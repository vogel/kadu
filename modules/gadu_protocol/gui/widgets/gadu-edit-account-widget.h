 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_EDIT_ACCOUNT_WIDGET_H
#define GADU_EDIT_ACCOUNT_WIDGET_H

#include "gui/widgets/account-edit-widget.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;

class ChooseIdentityWidget;
class GaduPersonalInfoWidget;
class ProxyGroupBox;

class GaduEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	QCheckBox *ConnectAtStart;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;

	ChooseIdentityWidget *ChooseIdentity;

	GaduPersonalInfoWidget *gpiw;
	ProxyGroupBox *proxy;

	QCheckBox *useDefaultServers;
	QLineEdit *ipAddresses;
	QComboBox *port;

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
	void importListAsFile();
	void contactListDownloaded(QString);

public:
	explicit GaduEditAccountWidget(Account account, QWidget *parent = 0);
	virtual ~GaduEditAccountWidget();

	virtual void apply();
	void protcooaccount();

};

#endif // GADU_EDIT_ACCOUNT_WIDGET_H
