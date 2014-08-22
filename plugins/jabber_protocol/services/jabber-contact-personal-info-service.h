/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_CONTACT_PERSONAL_INFO_SERVICE_H
#define JABBER_CONTACT_PERSONAL_INFO_SERVICE_H

#include <xmpp_vcard.h>

#include "protocols/services/contact-personal-info-service.h"

namespace XMPP
{
	class JabberVCardService;
}

class JabberContactPersonalInfoService : public ContactPersonalInfoService
{
	Q_OBJECT

	QPointer<XMPP::JabberVCardService> VCardService;
	Buddy CurrentBuddy;

private slots:
	void vCardDownloaded(bool ok, XMPP::VCard vCard);

public:
	explicit JabberContactPersonalInfoService(Account account, QObject *parent = 0);
	virtual ~JabberContactPersonalInfoService();

	void setVCardService(XMPP::JabberVCardService *vCardService);

	virtual void fetchPersonalInfo(Contact contact);

};

#endif // JABBER_CONTACT_PERSONAL_INFO_SERVICE_H

