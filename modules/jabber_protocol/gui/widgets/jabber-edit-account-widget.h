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

#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

#include "gui/widgets/account-edit-widget.h"

class QCheckBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;

class ChooseIdentityWidget;
class ProxyGroupBox;

class JabberEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	QCheckBox *ConnectAtStart;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	
	QCheckBox *CustomHostPort;
	QHBoxLayout *HostPortLayout;
	QLabel *CustomHostLabel;
	QLineEdit *CustomHost;
	QLabel *CustomPortLabel;
	QLineEdit *CustomPort;
	QLabel *EncryptionModeLabel;
	QComboBox *EncryptionMode;
	QCheckBox *LegacySSLProbe;
	QComboBox *PlainTextAuth;

	ChooseIdentityWidget *ChooseIdentity;
	ProxyGroupBox *proxy;

	QCheckBox *AutoResource;
	QHBoxLayout *ResourceLayout;
	QLabel *ResourceLabel;
	QLineEdit *ResourceName;
	QLabel *PriorityLabel;
	QLineEdit *Priority;
	
	QLabel *DataTransferProxyLabel;
	QLineEdit *DataTransferProxy;
	QHBoxLayout *DataTransferProxyLayout;

	void createGui();
	void createGeneralTab(QTabWidget *);
	void createPersonalDataTab(QTabWidget *);
	void createBuddiesTab(QTabWidget *);
	void createConnectionTab(QTabWidget *);
	void createOptionsTab(QTabWidget *);
	void createGeneralGroupBox(QVBoxLayout *layout);

	void loadAccountData();
	void loadConnectionData();
	bool checkSSL();

private slots:
	void removeAccount();
	void sslActivated(int i);
	void hostToggled(bool on);
	void autoResourceToggled(bool on);

public:
	explicit JabberEditAccountWidget(Account account, QWidget *parent = 0);
	virtual ~JabberEditAccountWidget();

	virtual void apply();

};

#endif // JABBER_EDIT_ACCOUNT_WIDGET_H
