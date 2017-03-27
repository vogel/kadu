/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "roster/roster-service.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class FacebookProtocol;
class QFacebookContact;
class QFacebookSession;
struct QFacebookDownloadContactsDeltaResult;
struct QFacebookDownloadContactsResult;

class BuddyManager;
class ContactManager;
class Contact;

class FacebookRosterService : public RosterService
{
	Q_OBJECT

public:
	explicit FacebookRosterService(QVector<Contact> contacts, Protocol &protocol, QFacebookSession &facebookSession);
	virtual ~FacebookRosterService();

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ContactManager> m_contactManager;

	QFacebookSession &m_facebookSession;

	void downloadRoster();
	void downloadFullRoster();
	void scheduleDownloadRoster();

	void contactsReceived(const QFacebookDownloadContactsResult &downloadContactsResult);
	void contactsDeltaReceived(const QFacebookDownloadContactsDeltaResult &downloadContactsDeltaResult);

	void replaceContacts(std::vector<QFacebookContact> newContacts);
	void addContact(const QFacebookContact &c);
	void removeContact(const QString &id);

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);

};
