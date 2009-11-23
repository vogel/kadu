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
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-search-service.h"

GaduSearchService::GaduSearchService(GaduProtocol *protocol) :
		SearchService(protocol), Protocol(protocol), Query(BuddySearchCriteria()),
		SearchSeq(0), From(0), Stopped(false)
{
}

void GaduSearchService::searchFirst(BuddySearchCriteria criteria)
{
	Query = criteria;
	From = Query.SearchBuddy.hasContact(Protocol->account())
			? Query.SearchBuddy.contact(Protocol->account()).id().toUInt()
			: 0;
	searchNext();
}

void GaduSearchService::searchNext()
{
	Stopped = false;
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

	if (Query.SearchBuddy.hasContact(Protocol->account()))
		gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(Query.SearchBuddy.contact(Protocol->account()).id()).data());
	if (!Query.SearchBuddy.firstName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(Query.SearchBuddy.firstName()).data());
	if (!Query.SearchBuddy.lastName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(Query.SearchBuddy.lastName()).data());
	if (!Query.SearchBuddy.nickName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(Query.SearchBuddy.nickName()).data());
	if (!Query.SearchBuddy.city().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(Query.SearchBuddy.city()).data());
	if (!Query.BirthYearFrom.isEmpty())
	{
		QString bufYear = Query.BirthYearFrom + ' ' + Query.BirthYearTo;
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)unicode2cp(bufYear).data());
	}
	switch (Query.SearchBuddy.gender())
	{
		case BuddyShared::GenderMale:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
			break;
		case BuddyShared::GenderFemale:
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

	BuddyList results;

	int count = gg_pubdir50_count(res);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "found %d results\n", count);

	for (int i = 0; i < count; i++)
	{
		Buddy result;

		Contact contact;
		contact.setContactAccount(Protocol->account());
		contact.setOwnerBuddy(result);
		contact.setId(gg_pubdir50_get(res, i, GG_PUBDIR50_UIN));
		contact.setDetails(new GaduContactDetails(contact));

		const char *pubdirStatus = gg_pubdir50_get(res, i, GG_PUBDIR50_STATUS);
		if (pubdirStatus)
		{	Status status;
			status.setType(Protocol->statusTypeFromGaduStatus(atoi(pubdirStatus) & 127));
			contact.setCurrentStatus(status);
		}

		result.addContact(contact);

		result.setFirstName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_FIRSTNAME)));
		result.setLastName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_LASTNAME)));
		result.setNickName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_NICKNAME)));
		result.setBirthYear(QString::fromAscii(gg_pubdir50_get(res, i, GG_PUBDIR50_BIRTHYEAR)).toUShort());
		result.setCity(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_CITY)));
		result.setFamilyName(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYNAME)));
		result.setFamilyCity(cp2unicode(gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYCITY)));
		result.setGender((BuddyShared::BuddyGender)QString::fromAscii(gg_pubdir50_get(res, i, GG_PUBDIR50_GENDER)).toUShort());

		results.append(result);
	}

	From = gg_pubdir50_next(res);

	emit newResults(results);
}
