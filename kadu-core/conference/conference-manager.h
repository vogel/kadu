/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFERENCE_MANAGER
#define CONFERENCE_MANAGER

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/accounts_aware_object.h"
#include "configuration/storable-object.h"

#include "conference.h"

class Account;
class XmlConfigFile;

class ConferenceManager : public QObject, public StorableObject, public AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ConferenceManager)

	static ConferenceManager * Instance;

	QList<Conference *> Conferences;

	ConferenceManager() {}
protected:
	virtual StoragePoint * createStoragePoint() const;

	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	static ConferenceManager * instance();

	unsigned int count() { return Conferences.count(); }

	void loadConfigurationForAccount(Account *account);
	void storeConfigurationForAccount(Account *account);

	void addConference(Conference *conference);
	void removeConference(Conference *conference);
	QList<Conference *> conferences() { return Conferences; }

signals:
	void conferenceAboutToBeAdded(Conference *conference);
	void conferenceAdded(Conference *conference);
	void conferenceAboutToBeRemoved(Conference *conference);
	void conferenceRemoved(Conference *conference);

};

#endif // CONFERENCE_MANAGER
