/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

	if (1 != node.count())
	{
		emit personalInfoAvailable(Buddy::null);
		return;
	}

	Buddy result;

	Contact contact;
	contact.setContactAccount(Protocol->account());
	contact.setOwnerBuddy(result);
	contact.setId(Protocol->account().id());
	contact.setDetails(new TlenContactDetails(contact));

	result.addContact(contact);

	/*
            <first>imie</first><last>nazwisko</last>
            <nick>nick</nick><email>email</email>
            <c>miesjcowosc</c><b>1984</b><s>1</s><e>szkola</e>
            <r>1</r><j>12</j><p>0</p><v>0</v><g>1</g><k>1</k>

	*/
	QDomElement itemelement = node.item(0).toElement();
	QDomNodeList items = itemelement.childNodes();
	for (int i=0;i<items.count();++i)
	{
		QDomElement mm = items.item(i).toElement();
		QString mmName = items.item(i).nodeName();
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "tlen node = %s\n", qPrintable(mm.text()));
		if (mmName == "first")
		{
			result.setFirstName(mm.text());
		}
		else if (mmName == "last")
		{
			result.setLastName(mm.text());
		}
		else if (mmName == "nick")
		{
			result.setNickName(mm.text());
		}
		else if (mmName == "email")
		{
			result.setEmail(mm.text());
		}
		else if (mmName == "b")
		{
			result.setBirthYear(mm.text().toUShort());
		}
		else if (mmName == "s")
		{
			result.setGender((BuddyShared::BuddyGender)mm.text().toUShort());
		}
		else if (mmName == "c")
		{
			result.setCity(mm.text());
		}
		else if (mmName == "r")
		{
			// searching for
		}
		else if (mmName == "j")
		{
			// job
		}
		else if (mmName == "p")
		{
			// my plan for 
		}
		else if (mmName == "v")
		{
			// status visible in catalog
		}
		else if (mmName == "g")
		{
			// mic
		}
		else if (mmName == "k")
		{
			// cam
		}

	}

	//result.setStatus();

	emit personalInfoAvailable(result);
}
/*
void TlenPersonalInfoService::handleEventPubdir50Write(struct gg_event *e)
{
	emit personalInfoUpdated(true);
}*/

void TlenPersonalInfoService::fetchPersonalInfo()
{
	kdebugf();
	client = Protocol->client();
	if (!client || !client->isConnected())
		return;

	connect(client, SIGNAL(pubdirReceived(QDomNodeList)), this, SLOT(handlePubdirReceived(QDomNodeList)));
	client->getPubDirInfoRequest();

}

void TlenPersonalInfoService::updatePersonalInfo(Buddy buddy)
{
	kdebugf();
/*	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_WRITE);

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
*/
}
