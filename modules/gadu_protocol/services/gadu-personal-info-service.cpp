/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-personal-info-service.h"

GaduPersonalInfoService::GaduPersonalInfoService(GaduProtocol *protocol) :
		PersonalInfoService(protocol), Protocol(protocol), FetchSeq(0), UpdateSeq(0)
{
}

void GaduPersonalInfoService::handleEventPubdir50Read(struct gg_event *e)
{
	gg_pubdir50_t res = e->event.pubdir50;

	if (FetchSeq != res->seq)
		return;

	Buddy result;

	int count = gg_pubdir50_count(res);
	if (1 != count)
	{
		emit personalInfoAvailable(Buddy::null);
		return;
	}

	Contact contact;
	contact.setContactAccount(Protocol->account());
	contact.setOwnerBuddy(result);
	contact.setId(gg_pubdir50_get(res, 0, GG_PUBDIR50_UIN));
	contact.setDetails(new GaduContactDetails(contact));

	result.addContact(contact);
	result.setFirstName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_FIRSTNAME)));
	result.setLastName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_LASTNAME)));
	result.setNickName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_NICKNAME)));
	result.setBirthYear(QString::fromAscii(gg_pubdir50_get(res, 0, GG_PUBDIR50_BIRTHYEAR)).toUShort());
	result.setCity(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_CITY)));
	result.setFamilyName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYNAME)));
	result.setFamilyCity(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYCITY)));
	result.setGender((BuddyShared::BuddyGender)QString::fromAscii(gg_pubdir50_get(res, 0, GG_PUBDIR50_GENDER)).toUShort());
	// TODO: 0.6.6
	// result.setStatus(gg_pubdir50_get(res, 0, GG_PUBDIR50_STATUS));

	emit personalInfoAvailable(result);
}

void GaduPersonalInfoService::handleEventPubdir50Write(struct gg_event *e)
{
	gg_pubdir50_t res = e->event.pubdir50;

	if (UpdateSeq != res->seq)
		return;

	emit personalInfoUpdated(true);
}

void GaduPersonalInfoService::fetchPersonalInfo()
{
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_READ);
	FetchSeq = gg_pubdir50(Protocol->gaduSession(), req);
	gg_pubdir50_free(req);
}

void GaduPersonalInfoService::updatePersonalInfo(Buddy buddy)
{
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_WRITE);

	if (!buddy.firstName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)(unicode2cp(buddy.firstName()).data()));
	if (!buddy.lastName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)(unicode2cp(buddy.lastName()).data()));
	if (!buddy.nickName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)(unicode2cp(buddy.nickName()).data()));
	if (!buddy.city().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)(unicode2cp(buddy.city()).data()));
	if (0 != buddy.birthYear())
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)(unicode2cp(QString::number(buddy.birthYear())).data()));
	// TODO: 0.6.6
	if (BuddyShared::GenderUnknown != buddy.gender())
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, (const char *)(unicode2cp(QString::number(buddy.gender())).data()));
	if (!buddy.familyName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)(unicode2cp(buddy.familyName()).data()));
	if (!buddy.familyCity().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, (const char *)(unicode2cp(buddy.familyCity()).data()));

	UpdateSeq = gg_pubdir50(Protocol->gaduSession(), req);
	gg_pubdir50_free(req);
}
