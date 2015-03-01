/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-contact-personal-info-service.h"

#include "jabber-vcard-downloader.h"
#include "jabber-vcard-service.h"

#include "buddies/buddy-manager.h"
#include "contacts/contact.h"

#include <QtCore/QDate>
#include <qxmpp/QXmppVCardIq.h>

JabberContactPersonalInfoService::JabberContactPersonalInfoService(Account account, QObject *parent) :
		ContactPersonalInfoService(account, parent)
{
}

JabberContactPersonalInfoService::~JabberContactPersonalInfoService()
{
}

void JabberContactPersonalInfoService::setVCardService(JabberVCardService *vCardService)
{
	VCardService = vCardService;
}

void JabberContactPersonalInfoService::fetchPersonalInfo(Contact contact)
{
	CurrentBuddy = Buddy::create();
	if (!VCardService)
		return;

	JabberVCardDownloader *vCardDownloader = VCardService.data()->createVCardDownloader();
	if (!vCardDownloader)
		return;

	connect(vCardDownloader, SIGNAL(vCardDownloaded(bool,QXmppVCardIq)), this, SLOT(vCardDownloaded(bool,QXmppVCardIq)));
	vCardDownloader->downloadVCard(contact.id());
}

void JabberContactPersonalInfoService::vCardDownloaded(bool ok, const QXmppVCardIq &vCard)
{
	if (!ok)
		return;

	CurrentBuddy.setNickName(vCard.nickName());
	CurrentBuddy.setFirstName(vCard.fullName());
	CurrentBuddy.setFamilyName(vCard.middleName());
	auto bday = vCard.birthday();
	if (bday.isValid() && !bday.isNull())
		CurrentBuddy.setBirthYear(bday.year());

	if (!vCard.addresses().isEmpty())
		CurrentBuddy.setCity(vCard.addresses().at(0).locality());
	if (!vCard.emails().isEmpty())
		CurrentBuddy.setEmail(vCard.emails().at(0).address());
	CurrentBuddy.setWebsite(vCard.url());

	emit personalInfoAvailable(CurrentBuddy);
}

#include "moc_jabber-contact-personal-info-service.cpp"
