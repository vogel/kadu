/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_CONTACT_DETAILS_H
#define JABBER_CONTACT_DETAILS_H

#include <xmpp_rosteritem.h>

#include "contacts/contact.h"
#include "contacts/contact-details.h"
#include "contacts/contact-shared.h"

class JabberContactDetails : public ContactDetails
{
  
	PROPERTY_DEC(XMPP::Subscription, ContactSubscription)

public:
	explicit JabberContactDetails(ContactShared *contactShared);
	virtual ~JabberContactDetails();

	virtual bool validateId();
	virtual void store();
	
	PROPERTY_DEF(XMPP::Subscription, subscription, setSubscription, ContactSubscription)

};

#endif // JABBER_CONTACT_DETAILS_H
