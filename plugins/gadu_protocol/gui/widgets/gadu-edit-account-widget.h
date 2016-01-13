/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "gui/widgets/account-edit-widget.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class ConfigurationManager;
class Configuration;
class ContactManager;
class GaduAccountDetails;
class GaduPersonalInfoWidget;
class GaduServersManager;
class IdentitiesComboBox;
class IdentityManager;
class ProxyComboBox;

class QCheckBox;
class QComboBox;
class QLineEdit;
class QSpinBox;
class QTabWidget;
class QVBoxLayout;

class GaduEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

public:
	explicit GaduEditAccountWidget(GaduServersManager *gaduServersManager, AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository, Account account, QWidget *parent = 0);
	virtual ~GaduEditAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<Configuration> m_configuration;
	QPointer<ContactManager> m_contactManager;
	QPointer<GaduServersManager> m_gaduServersManager;
	QPointer<IdentityManager> m_identityManager;

	GaduAccountDetails *Details;

	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;

	QCheckBox *ReceiveImagesDuringInvisibility;

	QCheckBox *ChatImageSizeWarning;

	IdentitiesComboBox *Identities;

	GaduPersonalInfoWidget *gpiw;

	QCheckBox *useDefaultServers;
	QLineEdit *ipAddresses;
	QCheckBox *UseTlsEncryption;

	QCheckBox *ShowStatusToEveryone;
	QCheckBox *SendTypingNotification;
	QCheckBox *ReceiveSpam;

	ProxyComboBox *ProxyCombo;

	QPushButton *ApplyButton;
	QPushButton *CancelButton;

	void createGui();

	void createGeneralTab(QTabWidget *);
	void createGeneralGroupBox(QVBoxLayout *layout);
	void createPersonalInfoTab(QTabWidget *);
	void createBuddiesTab(QTabWidget *);
	void createConnectionTab(QTabWidget *);
	void createOptionsTab(QTabWidget *);

	void loadAccountData();

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_INIT void init();

	virtual void removeAccount();
	void dataChanged();
	void remindUin();
	void remindPassword();
	void showStatusToEveryoneToggled(bool toggled);
	void stateChangedSlot(ConfigurationValueState state);

};
