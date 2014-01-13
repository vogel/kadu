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

#ifndef JABBER_VCARD_DOWNLOADER_H
#define JABBER_VCARD_DOWNLOADER_H

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
 * @class JabberVCardDownaloder
 * @short Class for downloading VCard.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for downloading VCard. To do that attach slot to vCardDownloaded() signal and call downloadVCard()
 * method. After VCard is downloaded vCardDownloaded() signal is emitted and this object deletes itself.
 */
class JabberVCardDownloader : public QObject
{
	Q_OBJECT

	QPointer<XMPP::Client> XmppClient;
	QPointer<XMPP::JT_VCard> Task;

	void done(XMPP::VCard vCard);
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
	explicit JabberVCardDownloader(XMPP::Client *client, QObject *parent = 0);
	virtual ~JabberVCardDownloader();

	/**
	 * @short Downloads VCard for given id.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id of contact to download VCard for
	 *
	 * Before calling this method attach to vCardDownloaded() signal to get informed about result. Please
	 * note that this method can be only called once. After that this object emits vCardDownloaded() and
	 * deletes itself.
	 */
	virtual void downloadVCard(const QString &id);

signals:
	/**
	 * @short Signal emitted when job of this class is done.
	 * @author Rafał 'Vogel' Malinowski
	 * @param ok success flag
	 * @param vCard downloaded VCard
	 *
	 * If ok is true then VCard downloading was successfull. If ok is false then operation failed.
	 */
	void vCardDownloaded(bool ok, XMPP::VCard vCard);
};

/**
 * @}
 */

#endif // JABBER_VCARD_DOWNLOADER_H
