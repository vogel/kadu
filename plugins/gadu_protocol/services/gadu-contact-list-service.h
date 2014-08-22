/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef GADU_CONTACT_LIST_SERVICE_H
#define GADU_CONTACT_LIST_SERVICE_H

#include "accounts/account.h"
#include "contacts/contact.h"
#include "protocols/services/contact-list-service.h"

class GaduContactListStateMachine;
class GaduConnection;
class RosterNotifier;

class GaduContactListService : public ContactListService
{
	Q_OBJECT

	QPointer<GaduConnection> Connection;
	QPointer<RosterNotifier> MyRosterNotifier;
	GaduContactListStateMachine *StateMachine;

	friend class GaduProtocolSocketNotifiers;
	void handleEventUserlist100Version(struct gg_event *e);
	void handleEventUserlist100PutReply(struct gg_event *e);
	void handleEventUserlist100GetReply(struct gg_event *e);
	void handleEventUserlist100Reply(struct gg_event *e);

	void putFinished(bool ok);
	void getFinished(bool ok);

private slots:
	void dirtyContactAdded(Contact contact);

public:
	explicit GaduContactListService(const Account &account, Protocol *protocol);
	virtual ~GaduContactListService();

	void setConnection(GaduConnection *connection);
	void setRosterNotifier(RosterNotifier *rosterNotifier);

	virtual bool haveToAskForAddingContacts() const;

	virtual void exportContactList(const BuddyList &buddies);

	virtual void copySupportedBuddyInformation(const Buddy &destination, const Buddy &source);

	virtual QList<Buddy> loadBuddyList(QTextStream &dataStream);
	virtual QByteArray storeBuddyList(const BuddyList &buddies);

public slots:
	virtual void exportContactList();
	virtual void importContactList();

signals:
	// state machine signals
	void stateMachinePutStarted();
	void stateMachinePutFinished();
	void stateMachinePutFailed();

	void stateMachineGetStarted();
	void stateMachineGetFinished();
	void stateMachineGetFailed();

	void stateMachineLocalDirty();
	void stateMachineRemoteDirty();

};

#endif // GADU_CONTACT_LIST_SERVICE_H
