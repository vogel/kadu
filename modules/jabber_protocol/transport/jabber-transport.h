/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

 /*
    Copyright (c) 2006      by Olivier Goffart  <ogoffart at kde.org>
    Kopete    (c) 2006 by the Kopete developers <kopete-devel@kde.org>
  */

#ifndef JABBER_TRANSPORT_H
#define JABBER_TRANSPORT_H

#include "accounts/account.h"
#include <QtCore/QObject>
// #include <kopeteaccount.h>

namespace XMPP {
	class Jid;
	class RosterItem;
}
//namespace Kopete { class StatusMessage; }
// class Account;
class JabberProtocol;

/**
 * this class handle a jabber gateway
 * @author Olivier Goffart */

class JabberTransport : public QObject//Kopete::Account
{
	Q_OBJECT

public:
	/**
	 * constructor called when the transport is created by info from server  (i.e not when loading kopete)
	 * @param parentAccount is the parent jabber account.
	 * @param item is the roster item of the gateway
	 * @param gateway_type eg: "msn" or "icq"  only used when the account is not loaded from config file for determining the icon
	 */
	JabberTransport (Account parentAccount, const XMPP::RosterItem &item, const QString& gateway_type=QString());

	/**
	 * constructor called when the transport is loaded from config
	 * @param parentAccount is the parent jabber account.
	 * @param accountId is the accountId
	 */
	JabberTransport (Account parentAccount, const QString &accountId );

	~JabberTransport ();

	/** Returns the action menu for this account. */
	//virtual void fillActionMenu( KActionMenu *actionMenu );

	/** Returns the action menu for this account. */
	virtual bool hasCustomStatusMenu() const;

	/** the parent account */
	Account account() const
	{ return m_account; }

	/* to get the protocol from the account */
	JabberProtocol *protocol () const;

	void connect(/* const Kopete::OnlineStatus& */) {}
	virtual void disconnect( ) {}

	/**
	 * called when the account is removed in the config ui
	 * will remove the subscription
	 */
	virtual bool removeAccount();


	enum TransportStatus { Normal , Creating, Removing , AccountRemoved };
	TransportStatus transportStatus() { return m_status; };

	/**
	 * return the legacyId conrresponding to the jid
	 *  example:  jhon%msn.com@msn.foojabber.org  ->  jhon@msn.com
	 */
	QString legacyId( const XMPP::Jid &jid );

public slots:

	/* Reimplemented from Kopete::Account */
	void setOnlineStatus(/* const Kopete::OnlineStatus& status , const Kopete::StatusMessage &reason = Kopete::StatusMessage()*/);
	void setStatusMessage(/* const Kopete::StatusMessage &statusMessage */);
	/**
	 * the account has been unregistered.
	 * loop over all contact and remove them
	 */
	void removeAllContacts();

	/**
	 * the Account has been removed from Kopete,  remove this account also
	 */
	void jabberAccountRemoved();

	/**
	 *  "eat" all contact in the account that have the same domain as us.
	 */
	void eatContacts();

protected:
	/**
	 * Create a new contact in the specified metacontact
	 *
	 * You shouldn't ever call this method yourself, For adding contacts see @ref addContact()
	 *
	 * This method is called by @ref Kopete::Account::addContact() in this method, you should
	 * simply create the new custom @ref Kopete::Contact in the given metacontact. You should
	 * NOT add the contact to the server here as this method gets only called when synchronizing
	 * the contact list on disk with the one in memory. As such, all created contacts from this
	 * method should have the "dirty" flag set.
	 *
	 * This method should simply be used to intantiate the new contact, everything else
	 * (updating the GUI, parenting to meta contact, etc.) is being taken care of.
	 *
	 * @param contactId The unique ID for this protocol
	 * @param parentContact The metacontact to add this contact to
	 */
	virtual bool createContact (const QString & contactID/*, Kopete::MetaContact parentContact*/);

private:
	Account m_account;
	TransportStatus m_status;

};

#endif
