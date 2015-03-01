/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"
#include "services/jabber-vcard-uploader.h"
#include "jabber-protocol.h"

#include "jabber-personal-info-service.h"

#include <qxmpp/QXmppVCardIq.h>

JabberPersonalInfoService::JabberPersonalInfoService(Account account, QObject *parent) :
		PersonalInfoService(account, parent)
{
}

JabberPersonalInfoService::~JabberPersonalInfoService()
{
}

void JabberPersonalInfoService::setVCardService(JabberVCardService *vCardService)
{
	VCardService = vCardService;
}

void JabberPersonalInfoService::fetchPersonalInfo(const QString &id)
{
	CurrentBuddy = Buddy::create();
	if (!VCardService)
		return;

	JabberVCardDownloader *vCardDownloader = VCardService->createVCardDownloader();
	if (!vCardDownloader)
		return;

	connect(vCardDownloader, SIGNAL(vCardDownloaded(bool,QXmppVCardIq)), this, SLOT(vCardDownloaded(bool,QXmppVCardIq)));
	vCardDownloader->downloadVCard(id);
}

void JabberPersonalInfoService::vCardDownloaded(bool ok, const QXmppVCardIq &vCard)
{
	if (!ok)
		return;

	CurrentBuddy.setNickName(vCard.nickName());
	CurrentBuddy.setFirstName(vCard.fullName());
	CurrentBuddy.setFamilyName(vCard.middleName());
	QDate bday = vCard.birthday();
	if (bday.isValid() && !bday.isNull())
		CurrentBuddy.setBirthYear(bday.year());

	if (!vCard.addresses().isEmpty())
		CurrentBuddy.setCity(vCard.addresses().at(0).locality());
	if (!vCard.emails().isEmpty())
		CurrentBuddy.setEmail(vCard.emails().at(0).address());
	CurrentBuddy.setWebsite(vCard.url());

	emit personalInfoAvailable(CurrentBuddy);
}

void JabberPersonalInfoService::updatePersonalInfo(const QString &id, Buddy buddy)
{
	Q_UNUSED(id);

	if (!VCardService)
	{
		emit personalInfoUpdated(false);
		return;
	}

	CurrentBuddy = buddy;

	auto  vcard = QXmppVCardIq{};
	vcard.setFullName(CurrentBuddy.firstName());
	vcard.setNickName(CurrentBuddy.nickName());
	vcard.setMiddleName(CurrentBuddy.familyName());
	QDate birthday;
	birthday.setDate(CurrentBuddy.birthYear(), 1, 1);
	vcard.setBirthday(birthday);

	auto addr = QXmppVCardAddress{};
	addr.setLocality(CurrentBuddy.city());
	vcard.setAddresses({addr});

	auto email = QXmppVCardEmail{};
	email.setAddress(CurrentBuddy.email());
	vcard.setEmails({email});

	vcard.setUrl(CurrentBuddy.website());

	auto vCardUploader = VCardService->createVCardUploader();
	if (!vCardUploader)
	{
		emit personalInfoUpdated(false);
		return;
	}

	vCardUploader->uploadVCard(vcard);
	emit personalInfoUpdated(true);
}

#include "moc_jabber-personal-info-service.cpp"
