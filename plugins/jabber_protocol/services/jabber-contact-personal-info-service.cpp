/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "contacts/contact.h"

#include "jabber-vcard-downloader.h"
#include "jabber-vcard-service.h"

#include "jabber-contact-personal-info-service.h"

JabberContactPersonalInfoService::JabberContactPersonalInfoService(Account account, QObject *parent) :
		ContactPersonalInfoService(account, parent)
{
}

JabberContactPersonalInfoService::~JabberContactPersonalInfoService()
{
}

void JabberContactPersonalInfoService::setVCardService(XMPP::JabberVCardService *vCardService)
{
	VCardService = vCardService;
}

void JabberContactPersonalInfoService::fetchPersonalInfo(Contact contact)
{
	CurrentBuddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
	if (!VCardService)
		return;

	JabberVCardDownloader *vCardDownloader = VCardService.data()->createVCardDownloader();
	if (!vCardDownloader)
		return;

	connect(vCardDownloader, SIGNAL(vCardDownloaded(bool,XMPP::VCard)), this, SLOT(vCardDownloaded(bool,XMPP::VCard)));
	vCardDownloader->downloadVCard(contact.id());
}

void JabberContactPersonalInfoService::vCardDownloaded(bool ok, XMPP::VCard vCard)
{
	if (!ok)
		return;

	CurrentBuddy.setNickName(vCard.nickName());
	CurrentBuddy.setFirstName(vCard.fullName());
	CurrentBuddy.setFamilyName(vCard.familyName());
	QDate bday = QDate::fromString(vCard.bdayStr(), "yyyy-MM-dd");
	if (bday.isValid() && !bday.isNull())
		CurrentBuddy.setBirthYear(bday.year());

	if (!vCard.addressList().isEmpty())
		CurrentBuddy.setCity(vCard.addressList().at(0).locality);
	if (!vCard.emailList().isEmpty())
		CurrentBuddy.setEmail(vCard.emailList().at(0).userid);
	CurrentBuddy.setWebsite(vCard.url());

	emit personalInfoAvailable(CurrentBuddy);
}

#include "moc_jabber-contact-personal-info-service.cpp"
