/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "misc.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu-search-service.h"

GaduSearchService::GaduSearchService(GaduProtocol *protocol) :
		SearchService(protocol), Protocol(protocol), Query(Contact::TypeNull),
		SearchSeq(0), From(0), Stopped(false)
{
	connect(Protocol->socketNotifiers(), SIGNAL(pubdirReplyReceived(gg_pubdir50_t)),
			this, SLOT(pubdirReplyReceived(gg_pubdir50_t)));
}

void GaduSearchService::searchFirst(Contact contact)
{
	Query = contact;
	From = 0;
	searchNext();
}

void GaduSearchService::searchNext()
{
	Stopped = false;

	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

// 	if (!Query.Uin.isEmpty()) TODO: 0.6.6
// 		gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(searchRecord.Uin).data());
	if (!Query.firstName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(Query.firstName()).data());
	if (!Query.lastName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(Query.lastName()).data());
	if (!Query.nickName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(Query.nickName()).data());
	if (!Query.city().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(Query.city()).data());
// 	if (!Query.BirthYearFrom.isEmpty()) TODO: 0.6.6
	if (0 != Query.birthYear())
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)unicode2cp(QString::number(Query.birthYear())).data());

	switch (Query.gender())
	{
		case ContactData::GenderMale:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
			break;
		case ContactData::GenderFemale:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
			break;
	}

	// TODO: 0.6.6
// 	if (searchRecord.Active)
// 		gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);

	gg_pubdir50_add(req, GG_PUBDIR50_START, qPrintable(QString::number(From)));

	SearchSeq = gg_pubdir50(Protocol->session(), req);
	gg_pubdir50_free(req);
}

void GaduSearchService::stop()
{
	Stopped = true;
}

void GaduSearchService::pubdirReplyReceived(gg_pubdir50_t res)
{
	ContactList results;

	int count = gg_pubdir50_count(res);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "found %d results\n", count);

	for (int i = 0; i < count; i++)
	{
		Contact result;

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

		results.append(result);
	}

	From = gg_pubdir50_next(res);

	emit newResults(results);
}
