/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
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

#ifndef JABBER_PROTOCOL_FACTORY_H
#define JABBER_PROTOCOL_FACTORY_H

#include "protocols/protocol-factory.h"
#include "status/status-adapter.h"

class JabberProtocolFactory : public ProtocolFactory
{
	Q_OBJECT
	Q_DISABLE_COPY(JabberProtocolFactory)

	static JabberProtocolFactory *Instance;
	QList<StatusType> SupportedStatusTypes;

	StatusAdapter *MyStatusAdapter;

public:
	static void createInstance();
	static void destroyInstance();

	static JabberProtocolFactory * instance() { return Instance; }

	virtual Protocol * createProtocolHandler(Account account);
	virtual AccountDetails * createAccountDetails(AccountShared *accountShared);
	virtual ContactDetails * createContactDetails(ContactShared *contactShared);
	virtual AccountAddWidget * newAddAccountWidget(bool showButtons, QWidget *parent);
	virtual AccountCreateWidget * newCreateAccountWidget(bool showButtons, QWidget *parent);
	virtual AccountEditWidget* newEditAccountWidget(Account, QWidget*);
	virtual QWidget * newContactPersonalInfoWidget(Contact contact, QWidget *parent = 0);
    virtual ProtocolMenuManager * protocolMenuManager();
	virtual QList<StatusType> supportedStatusTypes();
	virtual StatusAdapter * statusAdapter() { return MyStatusAdapter; }
	virtual QString idLabel();
	virtual QValidator::State validateId(QString id);
	virtual bool allowChangeServer();
	virtual QString defaultServer();
	virtual QString whatIsMyUsername() { return QString(); }

	virtual QString name() { return "jabber"; }
	virtual QString displayName() { return "Jabber/XMPP"; }

	virtual KaduIcon icon();

protected:
	JabberProtocolFactory();

};

#endif // JABBER_PROTOCOL_FACTORY_H
