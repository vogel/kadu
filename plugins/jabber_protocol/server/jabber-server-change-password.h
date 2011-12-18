/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#ifndef JABBER_SERVER_CHANGE_PASSWORD_H
#define JABBER_SERVER_CHANGE_PASSWORD_H

#include <QtCore/QObject>

#include "accounts/account.h"
#include "protocols/protocol.h"

class JabberServerChangePassword : public QObject
{
	Q_OBJECT

	bool Result;

	Account MyAccount;
	QString Password;
	QString NewPassword;

public:
	JabberServerChangePassword(Account account, const QString &password, const QString &newPassword);

	virtual void performAction();

	bool result() { return Result; }

private slots:
	void actionFinished();

signals:
	void finished(JabberServerChangePassword *);

};

#endif // JABBER_SERVER_CHANGE_PASSWORD_H
