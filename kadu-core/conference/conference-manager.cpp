/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "conference.h"
#include "conference-manager.h"

ConferenceManager * ConferenceManager::Instance = 0;

ConferenceManager *  ConferenceManager::instance()
{
	if (0 == Instance)
		Instance = new ConferenceManager();

	return Instance;
}

StoragePoint * ConferenceManager::createStoragePoint() const
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Conferences"));
}

void ConferenceManager::loadConfigurationForAccount(Account *account)
{
	if (!isValidStorage())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement transfersNewNode = storage()->point();

	if (transfersNewNode.isNull())
		return;

	QDomNodeList conferenceNodes = transfersNewNode.elementsByTagName("Conference");

	int count = conferenceNodes.count();

	QString uuid = account->uuid().toString();
	for (int i = 0; i < count; i++)
	{
		QDomElement conferenceElement = conferenceNodes.at(i).toElement();
		if (conferenceElement.isNull())
			continue;

		if (configurationStorage->getTextNode(conferenceElement, "Account") != uuid)
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(configurationStorage, conferenceElement);
		Conference *conference = Conference::loadFromStorage(contactStoragePoint);

		if (conference)
			addConference(conference);
// 		else TODO: remove?
// 			transfersNewNode.removeChild(conferenceElement);
	}
}

void ConferenceManager::storeConfigurationForAccount(Account *account)
{
	foreach (Conference *conference, Conferences)
		if (conference->account() == account)
			conference->storeConfiguration();
}

void ConferenceManager::addConference(Conference *conference)
{
	emit conferenceAboutToBeAdded(conference);
	Conferences.append(conference);
	emit conferenceAdded(conference);
}

void ConferenceManager::removeConference(Conference *conference)
{
	emit conferenceAboutToBeRemoved(conference);
	Conferences.removeAll(conference);
	conference->removeFromStorage();
	emit conferenceRemoved(conference);

	delete conference;
}

void ConferenceManager::accountRegistered(Account *account)
{
	loadConfigurationForAccount(account);
}

void ConferenceManager::accountUnregistered(Account *account)
{
	storeConfigurationForAccount(account);
}
