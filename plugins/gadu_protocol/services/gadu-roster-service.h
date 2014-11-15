/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#pragma once

#include "roster/roster-service.h"

struct gg_session;

class BuddyList;
class GaduConnection;
class GaduRosterStateMachine;
class RosterNotifier;

class GaduRosterService : public RosterService
{
	Q_OBJECT

public:
	explicit GaduRosterService(Account account, Protocol *protocol, const QVector<Contact> &contacts, QObject *parent = nullptr);
	virtual ~GaduRosterService();

	virtual bool supportsTasks() const override { return false; }

	void setConnection(GaduConnection *connection);
	void setRosterNotifier(RosterNotifier *rosterNotifier);

	void prepareRoster();

protected:
	virtual void executeTask(const RosterTask &task);

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

private:
	QPointer<GaduConnection> m_connection;
	QPointer<RosterNotifier> m_rosterNotifier;
	QPointer<RosterService> m_rosterService;
	GaduRosterStateMachine *m_stateMachine;
	QVector<Contact> m_exportedContacts;

	friend class GaduProtocolSocketNotifiers;
	void handleEventUserlist100Version(struct gg_event *e);
	void handleEventUserlist100PutReply(struct gg_event *e);
	void handleEventUserlist100GetReply(struct gg_event *e);
	void handleEventUserlist100Reply(struct gg_event *e);

	void putFinished(bool ok);
	void getFinished(bool ok);

	void exportContactList(const BuddyList &buddies);
	bool haveToAskForAddingContacts() const;

private slots:
	void exportContactList();
	void importContactList();
	void rosterChanged();

};
