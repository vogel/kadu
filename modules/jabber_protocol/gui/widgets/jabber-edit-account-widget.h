/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef JABBER_EDIT_ACCOUNT_WIDGET_H
#define JABBER_EDIT_ACCOUNT_WIDGET_H

#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "gui/widgets/account-edit-widget.h"
#include "gui/widgets/identities-combo-box.h"

#include "jabber-account-details.h"
#include "jabber-personal-info-widget.h"

class QCheckBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;

class ChooseIdentityWidget;
class ProxyGroupBox;

class JabberEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	JabberAccountDetails *AccountDetails;
	JabberPersonalInfoWidget *PersonalInfoWidget;

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

	IdentitiesComboBox *Identities;
	ProxyGroupBox *Proxy;

	QCheckBox *AutoResource;
	QHBoxLayout *ResourceLayout;
	QLabel *ResourceLabel;
	QLineEdit *ResourceName;
	QLabel *PriorityLabel;
	QLineEdit *Priority;

	QLabel *DataTransferProxyLabel;
	QLineEdit *DataTransferProxy;
	QHBoxLayout *DataTransferProxyLayout;

	QPushButton *ApplyButton;
	QPushButton *CancelButton;

	void createGui();
	void createGeneralTab(QTabWidget *);
	void createPersonalDataTab(QTabWidget *);
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
	void dataChanged();
	void changePasssword();
	void passwordChanged(const QString &newPassword);

	void showXmlConsole();

public:
	explicit JabberEditAccountWidget(Account account, QWidget *parent = 0);
	virtual ~JabberEditAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // JABBER_EDIT_ACCOUNT_WIDGET_H
