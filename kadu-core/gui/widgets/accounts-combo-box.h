/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "gui/widgets/actions-combo-box.h"

#include <injeqt/injeqt.h>

class AbstractAccountFilter;
class AccountManager;
class AccountsModel;
class AccountsProxyModel;
class InjectedFactory;

class AccountsComboBox : public ActionsComboBox
{
	Q_OBJECT

public:
	explicit AccountsComboBox(bool includeSelectAccount, ActionVisibility visibility, QWidget *parent = nullptr);
	virtual ~AccountsComboBox();

	void setCurrentAccount(Account account);
	Account currentAccount();

	void setIncludeIdInDisplay(bool includeIdInDisplay);

	void addFilter(AbstractAccountFilter *filter);
	void removeFilter(AbstractAccountFilter *filter);

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<InjectedFactory> m_injectedFactory;

	AccountsModel *Model;
	AccountsProxyModel *ProxyModel;

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

};
