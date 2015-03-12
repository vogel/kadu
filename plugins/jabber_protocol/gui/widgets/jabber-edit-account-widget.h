/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

#include "gui/widgets/account-edit-widget.h"
#include "gui/widgets/identities-combo-box.h"

#include "jabber-account-details.h"
#include "jabber-personal-info-widget.h"

class QCheckBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;

class ProxyComboBox;

class JabberEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	JabberAccountDetails *AccountDetails;
	JabberPersonalInfoWidget *PersonalInfoWidget;

	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;

	QCheckBox *CustomHostPort;
	QLabel *CustomHostLabel;
	QLineEdit *CustomHost;
	QLabel *CustomPortLabel;
	QLineEdit *CustomPort;
	QLabel *EncryptionModeLabel;
	QComboBox *EncryptionMode;
	QComboBox *PlainTextAuth;

	IdentitiesComboBox *Identities;

	QCheckBox *AutoResource;
	QLabel *ResourceLabel;
	QLineEdit *ResourceName;
	QLabel *PriorityLabel;
	QLineEdit *Priority;

	QLineEdit *DataTransferProxy;
	QCheckBox *RequireDataTransferProxy;

	QCheckBox *SendTypingNotification;
	QCheckBox *SendGoneNotification;

	QCheckBox *PublishSystemInfo;

	ProxyComboBox *ProxyCombo;

	QPushButton *ApplyButton;
	QPushButton *CancelButton;

	void createGui();

	void createGeneralTab(QTabWidget *);
	void createPersonalDataTab(QTabWidget *);
	void createConnectionTab(QTabWidget *);
	void createOptionsTab(QTabWidget *);
	void createGeneralGroupBox(QVBoxLayout *layout);

	void loadAccountData();
	void loadAccountDetailsData();

	void resetState();

private slots:
	virtual void removeAccount();
	void sslActivated(int i);
	void hostToggled(bool on);
	void autoResourceToggled(bool on);
	void dataChanged();
	void changePasssword();
	void passwordChanged(const QString &newPassword);
	void stateChangedSlot(ConfigurationValueState);

public:
	explicit JabberEditAccountWidget(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository, Account account, QWidget *parent = 0);
	virtual ~JabberEditAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // JABBER_EDIT_ACCOUNT_WIDGET_H
