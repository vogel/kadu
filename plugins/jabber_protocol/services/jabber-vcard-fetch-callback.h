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

#ifndef JABBER_VCARD_FETCH_CALLBACK_H
#define JABBER_VCARD_FETCH_CALLBACK_H

namespace XMPP
{

class VCard;

class JabberVCardService;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberVCardFetchCallback
 * @author Rafał 'Vogel' Malinowski
 * @short Callback object that is notified about finishing fetching a vcard from JabberVCardService.
 *
 * Override virtual vcardFetched method to get informed about finishing of fetching VCard action.
 */
class JabberVCardFetchCallback
{
	friend class JabberVCardService;

public:
	virtual ~JabberVCardFetchCallback() {}

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Callback method called when fetching of VCard data finished.
	 * @param ok true if fetching was successfull
	 * @param vcard fetched data
	 */
	virtual void vCardFetched(bool ok, const XMPP::VCard &vCard) = 0;

};

/**
 * @}
 */

}

#endif // JABBER_VCARD_FETCH_CALLBACK_H
