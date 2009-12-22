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

class ChooseIdentityWidget;
class JabberServerRegisterAccount;
class TokenWidget;

class JabberCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT
	
	QLineEdit *Username;
	QComboBox *Domain;
	QLineEdit *Password;
	QLineEdit *RetypePassword;
	QCheckBox *RememberPassword;
	QLabel *RemindPassword;
	ChooseIdentityWidget *Identity;

	QPushButton *ExpandConnectionOptionsButton;
	QGroupBox *ConnectionOptions;
	QCheckBox *CustomHostPort;
	QHBoxLayout *HostPortLayout;
	QLabel *CustomHostLabel;
	QLineEdit *CustomHost;
	QLabel *CustomPortLabel;
	QLineEdit *CustomPort;
	QLabel *EncryptionModeLabel;
	QComboBox *EncryptionMode;
	QCheckBox *LegacySSLProbe;

	int ssl_;
	bool opt_host_, legacy_ssl_probe_;
	QString host_;
	int port_;
	bool ShowConnectionOptions;
	
	void createGui();
	bool checkSSL();

private slots:
	void dataChanged();
	void registerAccountDataChanged();
	void registerNewAccount();
	void registerNewAccountFinished(JabberServerRegisterAccount *jsra);
	void connectionOptionsChanged();
	void hostToggled(bool on);
	void sslActivated(int i);
public:
	explicit JabberCreateAccountWidget(QWidget *parent = 0);
	virtual ~JabberCreateAccountWidget();

};

#endif // JABBER_CREATE_ACCOUNT_WIDGET_H
