/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef JABBER_ACCOUNT_DATA_MANAGER_H
#define JABBER_ACCOUNT_DATA_MANAGER_H

#include "configuration/account-data-manager.h"

class JabberAccountDetails;

class JabberAccountDataManager : public AccountDataManager
{
	Q_OBJECT

	JabberAccountDetails *Data;

public:
	JabberAccountDataManager(Account data);

	JabberAccountDetails * data() { return Data; }

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif // JABBER_ACCOUNT_DATA_MANAGER_H
