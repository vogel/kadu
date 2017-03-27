/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"
#include "widgets/account-edit-widget.h"

#include <injeqt/injeqt.h>

class ConfigurationManager;
class IconsManager;
class IdentitiesComboBox;
class PluginInjectedFactory;
class ProxyComboBox;

class QCheckBox;
class QLineEdit;
class QPushButton;

class FacebookEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

public:
	explicit FacebookEditAccountWidget(Account account, QWidget *parent = nullptr);
	virtual ~FacebookEditAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<IconsManager> m_iconsManager;
	QPointer<IdentityManager> m_identityManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

	owned_qptr<QLineEdit> m_accountId;
	owned_qptr<QLineEdit> m_password;
	owned_qptr<QCheckBox> m_rememberPassword;
	owned_qptr<IdentitiesComboBox> m_identities;
	owned_qptr<ProxyComboBox> m_proxies;
	owned_qptr<QPushButton> m_apply;
	owned_qptr<QPushButton> m_cancel;

	void createGui();
	void createGeneralTab(QTabWidget *);

	void loadAccountData();

	void resetState();

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_INIT void init();

	virtual void removeAccount();

	void dataChanged();
	void stateChangedSlot(ConfigurationValueState);

};
