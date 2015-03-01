/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class QXmppVCardIq;
class QXmppVCardManager;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberVCardUploader
 * @short Class for uploading VCard.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for uploading VCard. To do that attach slot to vCardUploaded() signal and call uploadVCard()
 * method. After VCard is uploaded vCardUploaded() signal is emitted and this object deletes itself.
 */
class JabberVCardUploader : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance attached to given Client.
	 * @param client instance of Client
	 * @param parent QObject parent
	 */
	explicit JabberVCardUploader(QXmppVCardManager *vcardManager, QObject *parent = nullptr);
	virtual ~JabberVCardUploader();

	/**
	 * @short Uploads VCard for current client.
	 * @author Rafał 'Vogel' Malinowski
	 * @param vCard VCard to upload
	 *
	 * Before calling this method attach to vCardUploaded() signal to get informed about result. Please
	 * note that this method can be only called once. After that this object emits vCardUploaded() and
	 * deletes itself.
	 */
	void uploadVCard(const QXmppVCardIq &vcard);

signals:
	void vCardUploaded(bool ok);

private:
	QPointer<QXmppVCardManager> m_vcardManager;

};

/**
 * @}
 */
