/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "jabber-protocol.h"
#include "utils/vcard-factory.h"

#include "jabber-personal-info-service.h"

JabberPersonalInfoService::JabberPersonalInfoService(JabberProtocol *protocol) :
		PersonalInfoService(protocol), Protocol(protocol)
{
}

void JabberPersonalInfoService::fetchPersonalInfo()
{
	CurrentBuddy = Buddy::create();
	VCardFactory::instance()->getVCard(Protocol->account().id(), Protocol->client()->rootTask(), this, SLOT(fetchingVCardFinished()));
}

void JabberPersonalInfoService::fetchingVCardFinished()
{
	XMPP::VCard vcard;
	JT_VCard *task = (JT_VCard *)sender();
	
	if (task && task->success())
	{
		vcard = task->vcard();
		CurrentBuddy.setNickName(vcard.nickName());
		CurrentBuddy.setFirstName(vcard.fullName());
		CurrentBuddy.setFamilyName(vcard.familyName());
		CurrentBuddy.setBirthYear(vcard.bday().year());
		if (vcard.addressList().count() > 0)
			CurrentBuddy.setCity(vcard.addressList().at(0).locality);
		if (vcard.emailList().count() > 0)
			CurrentBuddy.setEmail(vcard.emailList().at(0).userid);
		CurrentBuddy.setWebsite(vcard.url());
		
		emit personalInfoAvailable(CurrentBuddy);
	}
}

void JabberPersonalInfoService::updatePersonalInfo(Buddy buddy)
{
	CurrentBuddy = buddy;
	VCardFactory::instance()->getVCard(buddy.contacts(Protocol->account()).first().id(), Protocol->client()->rootTask(), this, SLOT(fetchingVCardFinished()));
}
