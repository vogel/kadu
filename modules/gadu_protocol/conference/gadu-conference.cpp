/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-list-configuration-helper.h"

#include "gadu-conference.h"

GaduConference::GaduConference(Account *currentAccount, ContactList contacts, QUuid uuid)
	: Conference(currentAccount, uuid), Contacts(contacts)
{
}

GaduConference::~GaduConference()
{
}

void GaduConference::loadConfiguration()
{
	if (!isValidStorage())
		return;

	Conference::loadConfiguration();
	QDomElement contactListNode = storage()->storage()->getNode(storage()->point(), "ContactList", XmlConfigFile::ModeFind);
	Contacts = ContactListConfigurationHelper::loadFromConfiguration(storage()->storage(), contactListNode);
}

void GaduConference::storeConfiguration()
{
	if (!isValidStorage())
		return;

	Conference::storeConfiguration();
	QDomElement contactListNode = storage()->storage()->getNode(storage()->point(), "ContactList", XmlConfigFile::ModeCreate);
	ContactListConfigurationHelper::saveToConfiguration(storage()->storage(), contactListNode, Contacts);
}


