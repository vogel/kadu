/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "exports.h"

#include <QtCore/QObject>

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

public:
	/**
	 * @short Returns account bounded with this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return account bounded with this service
	 */
	Account account() const;

protected:
	explicit AccountService(Account account, QObject *parent = nullptr);
	virtual ~AccountService();

private:
	Account m_account;

};

/**
 * @}
 */
