/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QDate>

#include <xmpp_jid.h>
#include <xmpp_vcard.h>

#include "buddies/buddy-manager.h"

#include "jabber-contact-personal-info-service.h"
#include "jabber-vcard-service.h"

JabberContactPersonalInfoService::JabberContactPersonalInfoService(QObject *parent) :
		ContactPersonalInfoService(parent)
{
}

JabberContactPersonalInfoService::~JabberContactPersonalInfoService()
{
}

void JabberContactPersonalInfoService::setVCardService(XMPP::JabberVCardService *vcardService)
{
	VCardService = vcardService;
}

void JabberContactPersonalInfoService::fetchPersonalInfo(Contact contact)
{
	CurrentBuddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
	if (VCardService)
		VCardService.data()->fetch(contact.id(), this);
}

void JabberContactPersonalInfoService::vcardFetched(bool ok, const XMPP::VCard &vcard)
{
	if (!ok)
		return;

	CurrentBuddy.setNickName(vcard.nickName());
	CurrentBuddy.setFirstName(vcard.fullName());
	CurrentBuddy.setFamilyName(vcard.familyName());
	QDate bday = QDate::fromString(vcard.bdayStr(), "yyyy-MM-dd");
	if (bday.isValid() && !bday.isNull())
		CurrentBuddy.setBirthYear(bday.year());

	if (!vcard.addressList().isEmpty())
		CurrentBuddy.setCity(vcard.addressList().at(0).locality);
	if (!vcard.emailList().isEmpty())
		CurrentBuddy.setEmail(vcard.emailList().at(0).userid);
	CurrentBuddy.setWebsite(vcard.url());

	emit personalInfoAvailable(CurrentBuddy);
}
