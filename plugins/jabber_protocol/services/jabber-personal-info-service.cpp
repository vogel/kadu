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

#include "utils/vcard-factory.h"
#include "jabber-protocol.h"

#include "jabber-personal-info-service.h"

JabberPersonalInfoService::JabberPersonalInfoService(JabberProtocol *protocol) :
		PersonalInfoService(protocol), Protocol(protocol)
{
}

void JabberPersonalInfoService::fetchPersonalInfo()
{
	CurrentBuddy = Buddy::create();
	if (Protocol && Protocol->client() && Protocol->client()->rootTask())
		VCardFactory::instance()->getVCard(Protocol->account().id(), Protocol->client()->rootTask(), this, SLOT(fetchingVCardFinished()));
}

void JabberPersonalInfoService::fetchingVCardFinished()
{
	XMPP::VCard vcard;
	XMPP::JT_VCard *task = (XMPP::JT_VCard *)sender();
	
	if (task && task->success())
	{
		vcard = task->vcard();
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
}

void JabberPersonalInfoService::updatePersonalInfo(Buddy buddy)
{
	if (!Protocol || !Protocol->client() || !Protocol->client()->rootTask())
		return;

	CurrentBuddy = buddy;
	
	XMPP::Jid jid = XMPP::Jid(Protocol->account().id());
	XMPP::VCard vcard;
	vcard.setFullName(CurrentBuddy.firstName());
	vcard.setNickName(CurrentBuddy.nickName());
	vcard.setFamilyName(CurrentBuddy.familyName());
	QDate birthday;
	birthday.setDate(CurrentBuddy.birthYear(), 1, 1);
	vcard.setBdayStr(birthday.toString("yyyy-MM-dd"));
	
	XMPP::VCard::Address addr;
	XMPP::VCard::AddressList addrList;
	addr.locality = CurrentBuddy.city();
	addrList.append(addr);
	vcard.setAddressList(addrList);
	
	XMPP::VCard::Email email;
	XMPP::VCard::EmailList emailList;
	email.userid = CurrentBuddy.email();
	emailList.append(email);
	vcard.setEmailList(emailList);
	
	vcard.setUrl(CurrentBuddy.website());
	
	VCardFactory::instance()->setVCard(Protocol->client()->rootTask(), jid, vcard, this, SLOT(uploadingVCardFinished()));
}

void JabberPersonalInfoService::uploadingVCardFinished()
{
	XMPP::JT_VCard *VCardHandler = static_cast<XMPP::JT_VCard *>(sender());
	emit personalInfoUpdated(VCardHandler->success());
}
