/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/misc.h"

#include "helpers/gadu-protocol-helper.h"
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

	int count = gg_pubdir50_count(res);
	if (1 != count)
	{
		emit personalInfoAvailable(Buddy::null);
		return;
	}

	Buddy result = GaduProtocolHelper::searchResultToBuddy(Protocol->account(), res, 0);

	// inverted values for "self" data
	// this is why gadu protocol suxx
	if (GenderMale == result.gender())
		result.setGender(GenderFemale);
	else if (GenderFemale == result.gender())
		result.setGender(GenderMale);

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
	Protocol->disableSocketNotifiers();
	FetchSeq = gg_pubdir50(Protocol->gaduSession(), req);
	Protocol->enableSocketNotifiers();
	//gg_pubdir50_free(req);
}

void GaduPersonalInfoService::updatePersonalInfo(Buddy buddy)
{
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_WRITE);

	if (!buddy.firstName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, buddy.firstName().toUtf8().constData());
	if (!buddy.lastName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, buddy.lastName().toUtf8().constData());
	if (!buddy.nickName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, buddy.nickName().toUtf8().constData());
	if (!buddy.city().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, buddy.city().toUtf8().constData());
	if (0 != buddy.birthYear())
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, QString::number(buddy.birthYear()).toUtf8().constData());

	// inverted values for "self" data
	// this is why gadu protocol suxx
	if (GenderMale == buddy.gender())
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, "1");
	else if (GenderFemale == buddy.gender())
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, "2");

	if (!buddy.familyName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, buddy.familyName().toUtf8().constData());
	if (!buddy.familyCity().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, buddy.familyCity().toUtf8().constData());

	Protocol->disableSocketNotifiers();
	UpdateSeq = gg_pubdir50(Protocol->gaduSession(), req);
	Protocol->enableSocketNotifiers();
	//gg_pubdir50_free(req);
}
