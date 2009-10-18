/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "misc/misc.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu-search-service.h"

GaduSearchService::GaduSearchService(GaduProtocol *protocol) :
		SearchService(protocol), Protocol(protocol), Query(ContactSearchCriteria()),
		SearchSeq(0), From(0), Stopped(false)
{
}

void GaduSearchService::searchFirst(ContactSearchCriteria criteria)
{
	Query = criteria;
	From = Query.SearchContact.hasAccountData(Protocol->account()) ? Query.SearchContact.accountData(Protocol->account())->id().toUInt() : 0;
	searchNext();
}

void GaduSearchService::searchNext()
{
	Stopped = false;
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

	if (Query.SearchContact.hasAccountData(Protocol->account()))
		gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(Query.SearchContact.accountData(Protocol->account())->id()).data());
	if (!Query.SearchContact.firstName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(Query.SearchContact.firstName()).data());
	if (!Query.SearchContact.lastName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(Query.SearchContact.lastName()).data());
	if (!Query.SearchContact.nickName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(Query.SearchContact.nickName()).data());
	if (!Query.SearchContact.city().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(Query.SearchContact.city()).data());
	if (!Query.BirthYearFrom.isEmpty())
	{
		QString bufYear = Query.BirthYearFrom + ' ' + Query.BirthYearTo;
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)unicode2cp(bufYear).data());
	}
	switch (Query.SearchContact.gender())
	{
		case ContactData::GenderMale:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
			break;
		case ContactData::GenderFemale:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
			break;
	}

	if (Query.Active)
		gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);

	gg_pubdir50_add(req, GG_PUBDIR50_START, qPrintable(QString::number(From)));

	SearchSeq = gg_pubdir50(Protocol->gaduSession(), req);
	gg_pubdir50_free(req);
}

void GaduSearchService::stop()
{
	Stopped = true;
}

void GaduSearchService::handleEventPubdir50SearchReply(struct gg_event *e)
{
	gg_pubdir50_t res = e->event.pubdir50;

	ContactList results;

	int count = gg_pubdir50_count(res);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "found %d results\n", count);

	for (int i = 0; i < count; i++)
	{
		Contact result;

		GaduContactAccountData *gcad = new GaduContactAccountData(Protocol->account(), result,
				gg_pubdir50_get(res, i, GG_PUBDIR50_UIN));
		//TODO 0.6.6
		Status status;
		status.setType(Protocol->statusTypeFromGaduStatus(atoi(gg_pubdir50_get(res, i, GG_PUBDIR50_STATUS)) & 127));
		gcad->setStatus(status);
		result.addAccountData(gcad);

		result.setFirstName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_FIRSTNAME)));
		result.setLastName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_LASTNAME)));
		result.setNickName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_NICKNAME)));
		result.setBirthYear(QString::fromAscii(gg_pubdir50_get(res, i, GG_PUBDIR50_BIRTHYEAR)).toUShort());
		result.setCity(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_CITY)));
		result.setFamilyName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYNAME)));
		result.setFamilyCity(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYCITY)));
		result.setGender((ContactData::ContactGender)QString::fromAscii(gg_pubdir50_get(res, i, GG_PUBDIR50_GENDER)).toUShort());

		results.append(result);
	}

	From = gg_pubdir50_next(res);

	emit newResults(results);
}
