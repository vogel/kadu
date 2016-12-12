/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "windows/open-chat-with/open-chat-with-runner.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyManager;
class ContactManager;

class JabberOpenChatWithRunner : public QObject, public OpenChatWithRunner
{
	Q_OBJECT

public:
	explicit JabberOpenChatWithRunner(Account account, QObject *parent = nullptr);
	virtual ~JabberOpenChatWithRunner();

	virtual BuddyList matchingContacts(const QString &query) override;

	void setAccount(Account account);

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ContactManager> m_contactManager;

	Account m_account;

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);

};
