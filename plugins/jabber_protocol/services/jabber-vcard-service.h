/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>

class JabberVCardDownloader;
class JabberVCardUploader;

class QXmppVCardManager;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberVCardService
 * @short Service for downloading and uploading VCard data.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service allows downloading and uploading VCard data. For each operation call createVCardDownloader() or
 * createVCardUploader() and use returned object to do its job. Each returner object can be used only once. Service
 * can return null values if given operation is not possible at time of call.
 *
 * This service requres Client instance for connecting with server. Use setXmppClient() method to provide one.
 */
class JabberVCardService : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of JabberVCardService.
	 * @param parent QObject parent
	 */
	explicit JabberVCardService(QXmppVCardManager *vcardManager, QObject *parent = 0);
	virtual ~JabberVCardService();

	/**
	 * @short Get JabberVCardDownloader for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return JabberVCardDownloader for this service
	 *
	 * This method will create and return JabberVCardDownloader class that can be used to download VCard for any contact.
	 * This method can return null if it is impossible to download a VCard.
	 *
	 * Returned instance should be used immediately and should not be stored for future use. Returned object will delete
	 * itself after one use, so next instance should be created in case first upload fails.
	 */
	JabberVCardDownloader * createVCardDownloader();

	/**
	 * @short Get JabberVCardUploader for this service.
	 * @author Rafał 'Vogel' Malinowski
	 * @return JabberVCardUploader for this service
	 *
	 * This method will create and return JabberVCardUploader class that can be used to upload new VCard for account owner.
	 * This method can return null if it is impossible to upload a VCard.
	 *
	 * Returned instance should be used immediately and should not be stored for future use. Returned object will delete
	 * itself after one use, so next instance should be created in case first upload fails.
	 */
	JabberVCardUploader * createVCardUploader();

private:
	QPointer<QXmppVCardManager> m_vcardManager;

};

/**
 * @}
 */
