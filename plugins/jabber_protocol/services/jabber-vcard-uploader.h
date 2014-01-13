/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_VCARD_UPLOADER_H
#define JABBER_VCARD_UPLOADER_H

#include <QtCore/QPointer>

#include <xmpp_vcard.h>

namespace XMPP
{
	class Client;
	class JT_VCard;
}

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

	QPointer<XMPP::Client> XmppClient;
	QPointer<XMPP::JT_VCard> Task;

	void done();
	void failed();

private slots:
	void taskFinished();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance attached to given XMPP::Client.
	 * @param client instance of XMPP::Client
	 * @param parent QObject parent
	 */
	explicit JabberVCardUploader(XMPP::Client *client, QObject *parent = 0);
	virtual ~JabberVCardUploader();

	/**
	 * @short Uploads VCard for given id.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id of contact to upload VCard for
	 * @param vCard VCard to upload
	 *
	 * Before calling this method attach to vCardUploaded() signal to get informed about result. Please
	 * note that this method can be only called once. After that this object emits vCardUploaded() and
	 * deletes itself.
	 */
	virtual void uploadVCard(const QString &id, XMPP::VCard vCard);

signals:
	/**
	 * @short Signal emitted when job of this class is done.
	 * @author Rafał 'Vogel' Malinowski
	 * @param ok success flag
	 *
	 * If ok is true then VCard uploading was successfull. If ok is false then operation failed.
	 */
	void vCardUploaded(bool ok);
};

/**
 * @}
 */

#endif // JABBER_VCARD_UPLOADER_H
