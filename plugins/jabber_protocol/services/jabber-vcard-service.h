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

#ifndef JABBER_VCARD_SERVICE_H
#define JABBER_VCARD_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QWeakPointer>

namespace XMPP
{

class Client;
class Jid;
class JT_VCard;
class VCard;

class JabberVCardFetchCallback;
class JabberVCardUpdateCallback;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberVCardService
 * @short Service for feteching and updating VCard data.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service allows feteching and updating VCard data. Each fetch and update action require a callback object of
 * JabberVCardFetchCallback or JabberVCardUpdateCallback type. Do not destroy these objects before callback method
 * is executed.
 *
 * This service requres XMPP::Client instance for connecting with server. Use setXmppClient() method to provide one.
 */
class JabberVCardService : public QObject
{
	Q_OBJECT

	QWeakPointer<XMPP::Client> XmppClient;
	QMap<JT_VCard *, JabberVCardFetchCallback *> FetchCallbacks;
	QMap<JT_VCard *, JabberVCardUpdateCallback *> UpdateCallbacks;

private slots:
	void fetched();
	void updated();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of JabberVCardService.
	 * @param parent QObject parent
	 */
	explicit JabberVCardService(QObject *parent = 0);
	virtual ~JabberVCardService();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets instance of XMPP::Client to use by this serivce.
	 * @param xmppClient instance of XMPP::Client to be used by this service
	 *
	 * There is no need to call setXmppClient(0) as this service is aware of object destroying.
	 * If no XMPP::Client is available all actions will fail.
	 */
	void setXmppClient(XMPP::Client *xmppClient);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns XMPP::Client instance used by this service.
	 * @return XMPP::Client instance used by this service
	 */
	XMPP::Client * xmppClient() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Fetch VCard data for given jid.
	 * @param jid jid to fetch data for
	 * @param callback callback object will receive finished notification
	 *
	 * This method will do nothing if no callback object is provided. If no valid XMPP::Client is availabe
	 * then this emthod will fail immediately and notify callback object.
	 *
	 * In other cases a new task will be send to XMPP server and notification will be issued after it is finished.
	 */
	void fetch(const XMPP::Jid &jid, JabberVCardFetchCallback *callback);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Update VCard data for given jid.
	 * @param jid jid to update data for
	 * @param vCard new VCard data for given jid
	 * @param callback callback object will receive finished notification
	 *
	 * This method will do nothing if no callback object is provided. If no valid XMPP::Client is availabe
	 * then this emthod will fail immediately and notify callback object.
	 *
	 * In other cases a new task will be send to XMPP server and notification will be issued after it is finished.
	 */
	void update(const XMPP::Jid &jid, XMPP::VCard vCard, JabberVCardUpdateCallback *callback);

};

/**
 * @}
 */

}

#endif // JABBER_VCARD_SERVICE_H
