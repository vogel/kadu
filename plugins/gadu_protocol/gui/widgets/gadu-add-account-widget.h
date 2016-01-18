/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/account-add-widget.h"

#include <injeqt/injeqt.h>

class AccountManager;
class AccountStorage;
class IdentitiesComboBox;
class IdentityManager;
class InjectedFactory;

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QPushButton;

class GaduAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

public:
	explicit GaduAddAccountWidget(bool showButtons, QWidget *parent = 0);
	virtual ~GaduAddAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<AccountStorage> m_accountStorage;
	QPointer<IdentityManager> m_identityManager;
	QPointer<InjectedFactory> m_injectedFactory;

	bool m_showButtons;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	IdentitiesComboBox *Identity;
	QPushButton *AddAccountButton;

	void createGui(bool showButtons);
	void resetGui();

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setAccountStorage(AccountStorage *accountStorage);
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void dataChanged();
	void registerAccount();
	void remindUin();
	void remindPassword();

};
