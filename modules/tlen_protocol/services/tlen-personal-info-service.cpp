/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/misc.h"
#include "debug.h"

#include "tlen.h"

#include "tlen-contact-details.h"
#include "tlen-protocol.h"

#include "tlen-personal-info-service.h"

TlenPersonalInfoService::TlenPersonalInfoService(TlenProtocol *protocol) :
		PersonalInfoService(protocol), Protocol(protocol)
{
}

void TlenPersonalInfoService::handlePubdirReceived(QDomNodeList node)
{
	kdebugf();

	client = Protocol->client();
	if (!client)
		return;
	
	disconnect(client, SIGNAL(pubdirReceived(QDomNodeList)), this, SLOT(handlePubdirReceived(QDomNodeList)));
	disconnect(client, SIGNAL(pubdirUpdated(bool)), this, SIGNAL(personalInfoUpdated(bool)));

	if (1 != node.count())
	{
		emit personalInfoAvailable(Buddy::null);
		return;
	}

	emit personalInfoAvailable(Protocol->nodeToBuddy(node.item(0)));
}

void TlenPersonalInfoService::fetchPersonalInfo()
{
	kdebugf();
	client = Protocol->client();
	if (!client || !client->isConnected())
		return;

	connect(client, SIGNAL(pubdirReceived(QDomNodeList)), this, SLOT(handlePubdirReceived(QDomNodeList)));
	connect(client, SIGNAL(pubdirUpdated(bool)), this, SIGNAL(personalInfoUpdated(bool)));
	client->getPubDirInfoRequest();

}

void TlenPersonalInfoService::updatePersonalInfo(Buddy buddy)
{
	kdebugf();

	client = Protocol->client();
	if (!client || !client->isConnected())
		return;

	// always 1 contact
	Contact contact = buddy.contacts()[0];
	if (contact.isNull())
	{
		emit personalInfoUpdated(false);
		return;
	}

	TlenContactDetails *tlenDetails = dynamic_cast<TlenContactDetails *>(contact.details());
	if (!tlenDetails)
	{
		emit personalInfoUpdated(false);
		return;
	}

	// TODO add email!!
	client->setPubDirInfo(buddy.firstName(), buddy.lastName(), buddy.nickName(), buddy.email(),
		 buddy.city(), buddy.birthYear(), (int)buddy.gender(),
		(int) tlenDetails->lookingFor(), (int) tlenDetails->job(),
		(int) tlenDetails->todayPlans(), tlenDetails->showStatus(),
		tlenDetails->haveMic(), tlenDetails->haveCam());
}
