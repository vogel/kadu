/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "contacts/contact-shared.h"

#include "tlen-contact-details.h"

class ContactShared;

TlenContactDetails::TlenContactDetails(ContactShared *contactShared) :
		ContactDetails(contactShared), TlenProtocolVersion(0),
		LookingFor(0), Job(0), TodayPlans(0),
		ShowStatus(false), HaveMic(false), HaveCam(false)
{
}

TlenContactDetails::~TlenContactDetails()
{
}

bool TlenContactDetails::validateId()
{
	return true;
}

void TlenContactDetails::load()
{
	if (!isValidStorage())
		return;

	ContactDetails::load();

	TlenProtocolVersion = loadValue<unsigned int>("TlenProtocolVersion");
	LookingFor = loadValue<unsigned int>("LookingFor");
	Job = loadValue<unsigned int>("Job");
	TodayPlans = loadValue<unsigned int>("TodayPlans");
	ShowStatus = loadValue<bool>("ShowStatus");
	HaveMic = loadValue<bool>("HaveMic");
	HaveCam = loadValue<bool>("HaveCam");
}

void TlenContactDetails::store()
{
	if (!isValidStorage())
		return;

	storeValue("TlenProtocolVersion", TlenProtocolVersion);
	storeValue("LookingFor", LookingFor);
	storeValue("Job", Job);
	storeValue("TodayPlans", TodayPlans);
	storeValue("ShowStatus", ShowStatus);
	storeValue("HaveMic", HaveMic);
	storeValue("HaveCam", HaveCam);
}
