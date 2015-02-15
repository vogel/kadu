/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contacts/contact.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

struct gg_session;

class GaduConnection;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduNotifyService
 * @short Service for sending gg_notify messages to GG server.
 *
 * gg_notify messages must be sent to GG server to let it know in which presences we are interested.
 * After every connection gg_notify_ex must be sent (even when contacts list is empty) with list of
 * all our contacts and their notify modes (buddy, friend, blocked).
 *
 * After adding/removing contacts from roster or after change of notify mode, a set of gg_add_notify_ex
 * and gg_remove_notify_ex messages must be sent to update status.
 *
 * This service is connected to RosterService to get update information about notify modes and new and
 * removed contacts.
 */
class GaduNotifyService : public QObject
{
	Q_OBJECT

public:
	/**
	 * @return GG notify mode for given contact
	 *
	 * Return value of this method is based on blocked and offline to contact properties.
	 */
	static int notifyTypeFromContact(const Contact &contact);

	/**
	 * @short Create new service that uses @p connection to send notify messages.
	 */
	explicit GaduNotifyService(GaduConnection *connection, QObject *parent = nullptr);
	virtual ~GaduNotifyService();

	/**
	 * @short Send gg_notify_ex with initial data for all provided contacts.
	 * @pre all contacts account must be the same as connection from constructor
	 *
	 * This method must be called immediately after GG connection has been made.
	 */
	void sendInitialData(const QVector<Contact> &contacts);

public slots:
	void contactAdded(Contact contact);
	void contactRemoved(Contact contact);
	void contactUpdatedLocally(Contact contact);

private:
	QPointer<GaduConnection> m_connection;

	bool updateFlag(gg_session *session, int uin, int newFlags, int oldFlags, int flag) const;
	void sendNewFlags(const Contact &contact, int newFlags) const;

};

/**
 * @}
 */
