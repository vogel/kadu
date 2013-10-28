/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QHistoryState>
#include <QtCore/QStringList>

#include "protocols/protocol.h"
#include "debug.h"

#include "services/gadu-contact-list-service.h"

#include "gadu-contact-list-state-machine.h"

GaduContactListStateMachine::GaduContactListStateMachine(GaduContactListService *service, Protocol *protocol) :
		QStateMachine(service), CurrentService(service)
{
	OfflineState = new QState(this);
	AwaitingServerGetResponseState = new QState(this);
	AwaitingServerPutResponseState = new QState(this);
	NormalState = new QState(this);

	connect(OfflineState, SIGNAL(entered()), SLOT(printConfiguration()));
	connect(AwaitingServerGetResponseState, SIGNAL(entered()), SLOT(printConfiguration()));
	connect(AwaitingServerPutResponseState, SIGNAL(entered()), SLOT(printConfiguration()));
	connect(NormalState, SIGNAL(entered()), SLOT(printConfiguration()));

	connect(AwaitingServerGetResponseState, SIGNAL(entered()), SIGNAL(awaitingServerGetResponseStateEntered()));
	connect(AwaitingServerPutResponseState, SIGNAL(entered()), SIGNAL(awaitingServerPutResponseStateEntered()));

	OfflineState->addTransition(protocol, SIGNAL(connected(Account)), AwaitingServerGetResponseState);

	AwaitingServerGetResponseState->addTransition(protocol, SIGNAL(disconnected(Account)), OfflineState);
	AwaitingServerGetResponseState->addTransition(CurrentService, SIGNAL(stateMachineFinishedImporting()), NormalState);

	AwaitingServerPutResponseState->addTransition(protocol, SIGNAL(disconnected(Account)), OfflineState);
	AwaitingServerPutResponseState->addTransition(CurrentService, SIGNAL(stateMachineSucceededExporting()), NormalState);
	AwaitingServerPutResponseState->addTransition(CurrentService, SIGNAL(stateMachineFailedExporting()), AwaitingServerGetResponseState);

	NormalState->addTransition(protocol, SIGNAL(disconnected(Account)), OfflineState);
	NormalState->addTransition(CurrentService, SIGNAL(stateMachineNewVersionAvailable()), AwaitingServerGetResponseState);
	NormalState->addTransition(CurrentService, SIGNAL(stateMachineHasDirtyContacts()), AwaitingServerPutResponseState);

	if (protocol->isConnected())
		setInitialState(AwaitingServerGetResponseState);
	else
		setInitialState(OfflineState);
}

GaduContactListStateMachine::~GaduContactListStateMachine()
{
}

void GaduContactListStateMachine::printConfiguration()
{
	QStringList states;

	if (configuration().contains(OfflineState))
		states.append("offline");
	if (configuration().contains(AwaitingServerGetResponseState))
		states.append("awaiting-server-get-response");
	if (configuration().contains(AwaitingServerPutResponseState))
		states.append("awaiting-server-put-response");
	if (configuration().contains(NormalState))
		states.append("normal");

	kdebugm(KDEBUG_INFO, "Gadu contact list state machine: [%s]\n", qPrintable(states.join(", ")));
}

bool GaduContactListStateMachine::awaitingServerGetResponse() const
{
	return configuration().contains(AwaitingServerGetResponseState);
}

bool GaduContactListStateMachine::awaitingServerPutResponse() const
{
	return configuration().contains(AwaitingServerPutResponseState);
}

#include "moc_gadu-contact-list-state-machine.cpp"
