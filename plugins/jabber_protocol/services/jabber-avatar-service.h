/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>

#include "protocols/services/avatar-service.h"

class JabberVCardService;

class AvatarManager;
class ContactManager;

class QXmppClient;
class QXmppPresence;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarService
 * @short Service for downloading and uploading avatars for XMPP/Jabber protocol.
 * @see AvatarService
 * @author Rafał 'Vogel' Malinowski
 *
 * See documentation of AvatarService to get general information about this service.
 *
 * JabberAvatarService uses JabberVCardService to create AvatarDownloader and
 * AvatarUploader instances. Use setVCardService() to set this service.
 */
class JabberAvatarService : public AvatarService
{
	Q_OBJECT

public:
	/**
	 * @short Create service instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param account account of service
	 * @param parent QObject parent of service
	 */
	explicit JabberAvatarService(QXmppClient *client, Account account, QObject *parent = 0);
	virtual ~JabberAvatarService();

	void setAvatarManager(AvatarManager *avatarManager);
	void setContactManager(ContactManager *contactManager);

	/**
	 * @short Set VCard service object to use in this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @param vCardService VCard service object to use
	 */
	void setVCardService(JabberVCardService *vCardService);

	virtual AvatarDownloader * createAvatarDownloader() override;
	virtual AvatarUploader * createAvatarUploader() override;
	virtual bool eventBasedUpdates() override { return true; }

private:
	QPointer<AvatarManager> m_avatarManager;
	QPointer<ContactManager> m_contactManager;

	QPointer<QXmppClient> m_client;
	QPointer<JabberVCardService> VCardService;

private slots:
    void rosterReceived();
	void presenceReceived(const QXmppPresence &presence);

};

/**
 * @}
 */
