/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef OTR_IS_LOGGED_IN_SERVICE_H
#define OTR_IS_LOGGED_IN_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

class Account;
class ContactManager;

class OtrIsLoggedInService : public QObject
{
	Q_OBJECT

	QPointer<ContactManager> CurrentContactManager;

public:
	enum IsLoggedInStatus
	{
		NotSure = -1,
		NotLoggedIn = 0,
		LoggedIn = 1
	};

	static int wrapperOtrIsLoggedIn(void *data, const char *accountName, const char *protocol, const char *recipient);

	explicit OtrIsLoggedInService(QObject *parent = 0);
	virtual ~OtrIsLoggedInService();

	void setContactManager(ContactManager *contactManager);

	IsLoggedInStatus isLoggedIn(const Account &account, const QString &contactId);

};

#endif // OTR_IS_LOGGED_IN_SERVICE_H
