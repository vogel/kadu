/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACCOUNT_TALKABLE_FILTER_H
#define ACCOUNT_TALKABLE_FILTER_H

#include "accounts/account.h"

#include "talkable/filter/talkable-filter.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class AccountTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that removes values that do not belong to specified account.
 *
 * This filter removes values that do not belong to specified account. Chat and Contacts are removed
 * if their accounts are different than specified one. Buddy instances are removed if it does not
 * contain any Contact with given Account. Other items are passed to next filters.
 */
class AccountTalkableFilter : public TalkableFilter
{
	Q_OBJECT

	Account FilterAccount;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of TalkableFilter with given parent.
	 * @param filterAccount account parameter of filter
	 * @param parent QObject parent of new object
	 *
	 * Create new instance of AccountTalkableFilter with given parent. Every item that does not belong
	 * to this account will get Rejected result. Other items will get Undecided result.
	 */
	explicit AccountTalkableFilter(const Account &filterAccount, QObject *parent = 0);
	virtual ~AccountTalkableFilter();

	virtual FilterResult filterChat(const Chat &chat);
	virtual FilterResult filterBuddy(const Buddy &buddy);
	virtual FilterResult filterContact(const Contact &contact);

};

/**
 * @}
 */

#endif // ACCOUNT_TALKABLE_FILTER_H
