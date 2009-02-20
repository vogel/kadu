/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu-personal-info-service.h"

GaduPersonalInfoService::GaduPersonalInfoService(GaduProtocol *protocol) :
		PersonalInfoService(protocol), Protocol(protocol), FetchSeq(0), UpdateSeq(0)
{
	connect(Protocol->socketNotifiers(), SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
			this, SLOT(pubdirReplyReceived(gg_pubdir50_t)));
}

void GaduPersonalInfoService::pubdirReplyReceived(gg_pubdir50_t res)
{
	if (FetchSeq == res->seq)
		fetchReplyReceived(res);
	else if (UpdateSeq == res->seq)
		updateReplyReceived(res);
}

void GaduPersonalInfoService::fetchReplyReceived(gg_pubdir50_t res)
{
	Contact result;
	
	int count = gg_pubdir50_count(res);
	if (1 != count)
	{
		emit personalInfoAvailable(Contact::null);
		return;
	}

	GaduContactAccountData *gcad = new GaduContactAccountData(result, Protocol->account(),
			gg_pubdir50_get(res, 0, GG_PUBDIR50_UIN));

	result.addAccountData(gcad);
	result.setFirstName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_FIRSTNAME)));
	result.setLastName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_LASTNAME)));
	result.setNickName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_NICKNAME)));
	result.setBirthYear(QString::fromAscii(gg_pubdir50_get(res, 0, GG_PUBDIR50_BIRTHYEAR)).toUShort());
	result.setCity(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_CITY)));
	result.setFamilyName(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYNAME)));
	result.setFamilyCity(cp2unicode(gg_pubdir50_get(res, 0, GG_PUBDIR50_FAMILYCITY)));
	result.setGender((ContactData::ContactGender)QString::fromAscii(gg_pubdir50_get(res, 0, GG_PUBDIR50_GENDER)).toUShort());
	// TODO: 0.6.6
	// result.setStatus(gg_pubdir50_get(res, 0, GG_PUBDIR50_STATUS));

	emit personalInfoAvailable(result);
}

void GaduPersonalInfoService::updateReplyReceived(gg_pubdir50_t res)
{
	emit personalInfoUpdated(true);
}

void GaduPersonalInfoService::fetchPersonalInfo()
{
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_READ);
	FetchSeq = gg_pubdir50(Protocol->session(), req);
	gg_pubdir50_free(req);
}

void GaduPersonalInfoService::updatePersonalInfo(Contact contact)
{
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_WRITE);

	if (!contact.firstName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)(unicode2cp(contact.firstName()).data()));
	if (!contact.lastName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)(unicode2cp(contact.lastName()).data()));
	if (!contact.nickName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)(unicode2cp(contact.nickName()).data()));
	if (!contact.city().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)(unicode2cp(contact.city()).data()));
	if (0 != contact.birthYear())
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)(unicode2cp(QString::number(contact.birthYear())).data()));
	// TODO: 0.6.6
	if (ContactData::GenderUnknown != contact.gender())
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, (const char *)(unicode2cp(QString::number(contact.gender())).data()));
	if (!contact.familyName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)(unicode2cp(contact.familyName()).data()));
	if (!contact.familyCity().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, (const char *)(unicode2cp(contact.familyCity()).data()));

	UpdateSeq = gg_pubdir50(Protocol->session(), req);
	gg_pubdir50_free(req);
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 4;  replace-tabs off;  replace-tabs off;
