/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef ACCOUNTS_COMBO_BOX_H
#define ACCOUNTS_COMBO_BOX_H

#include "accounts/account.h"
#include "gui/widgets/actions-combo-box.h"

class AbstractAccountFilter;
class AccountsModel;
class AccountsProxyModel;

class AccountsComboBox : public ActionsComboBox
{
	Q_OBJECT

	AccountsModel *Model;
	AccountsProxyModel *ProxyModel;

public:
	explicit AccountsComboBox(bool includeSelectAccount, ActionVisibility visibility, QWidget *parent = 0);
	virtual ~AccountsComboBox();

	void setCurrentAccount(Account account);
	Account currentAccount();

	void setIncludeIdInDisplay(bool includeIdInDisplay);

	void addFilter(AbstractAccountFilter *filter);
	void removeFilter(AbstractAccountFilter *filter);

};

#endif // ACCOUNTS_COMBO_BOX_H
