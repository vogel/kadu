/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACCOUNT_SERVICE_H
#define ACCOUNT_SERVICE_H

#include <QtCore/QObject>

#include "accounts/account.h"
#include "exports.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class AccountService
 * @author Rafał 'Vogel' Malinowski
 * @short Generic account service.
 *
 * All account services should derive from this class.
 */
class KADUAPI AccountService : public QObject
{
	Q_OBJECT

protected:
	Account ServiceAccount;

	explicit AccountService(Account account, QObject *parent = 0);
	virtual ~AccountService();

public:
	/**
	 * @short Returns account bounded with this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return account bounded with this service
	 */
	Account account() const;
};

/**
 * @}
 */

#endif // ACCOUNT_SERVICE_H
