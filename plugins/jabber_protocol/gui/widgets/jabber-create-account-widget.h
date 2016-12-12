/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "widgets/account-create-widget.h"
#include "widgets/identities-combo-box.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class AccountManager;
class AccountStorage;
class IconsManager;
class IdentityManager;
class PluginInjectedFactory;
class JabberServersService;
class Jid;

class QGridLayout;
class QLineEdit;
class QPushButton;

class JabberCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

public:
	explicit JabberCreateAccountWidget(bool showButtons, QWidget *parent = nullptr);
	virtual ~JabberCreateAccountWidget();

	void setJabberServersService(JabberServersService *serversService);

public slots:
	virtual void apply();
	virtual void cancel();

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<AccountStorage> m_accountStorage;
	QPointer<IconsManager> m_iconsManager;
	QPointer<IdentityManager> m_identityManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

	bool m_showButtons;
	QLineEdit *Username;
	QComboBox *Domain;
	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	QCheckBox *RememberPassword;
	QLineEdit *EMail;
	IdentitiesComboBox *IdentityCombo;
	QPushButton *RegisterAccountButton;

	void createGui(bool showButtons);
	void resetGui();

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setAccountStorage(AccountStorage *accountStorage);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_INIT void init();

	void dataChanged();
	void jidRegistered(const Jid &jid);

};
