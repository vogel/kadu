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

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance attached to given Client.
	 * @param client instance of Client
	 * @param parent QObject parent
	 */
	explicit JabberVCardDownloader(QXmppVCardManager *vcardManager, QObject *parent = nullptr);
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
	void vCardDownloaded(bool ok, const QXmppVCardIq &vcard);

private:
	QPointer<QXmppVCardManager> m_vcardManager;
	QString m_requestId;

	void done(const QXmppVCardIq &vcard);
	void failed();

private slots:
	void vCardReceived(const QXmppVCardIq &vcard);

};

/**
 * @}
 */
