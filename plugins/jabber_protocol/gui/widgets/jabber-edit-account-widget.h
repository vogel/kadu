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

#pragma once

#include "jabber-account-details.h"
#include "jabber-personal-info-widget.h"

#include "gui/widgets/account-edit-widget.h"
#include "gui/widgets/identities-combo-box.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <injeqt/injeqt.h>

class AccountManager;
class ConfigurationManager;
class IconsManager;
class IdentityManager;
class PluginInjectedFactory;
class ProxyComboBox;

class QCheckBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;

class JabberEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

public:
	explicit JabberEditAccountWidget(Account account, QWidget *parent = nullptr);
	virtual ~JabberEditAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<IconsManager> m_iconsManager;
	QPointer<IdentityManager> m_identityManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

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
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_INIT void init();

	virtual void removeAccount();

	void hostToggled(bool on);
	void autoResourceToggled(bool on);
	void dataChanged();
	void changePasssword();
	void passwordChanged(const QString &newPassword);
	void stateChangedSlot(ConfigurationValueState);

};
