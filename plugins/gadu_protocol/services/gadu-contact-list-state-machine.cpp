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
	auto globalState = new QState(ParallelStates);

	WorkState = new QState(globalState);
	OfflineState = new QState(WorkState);
	IdleState = new QState(WorkState);
	PutState = new QState(WorkState);
	GetState = new QState(WorkState);

	OfflineState->addTransition(protocol, SIGNAL(connected(Account)), IdleState);
	IdleState->addTransition(service, SIGNAL(stateMachinePutStarted()), PutState);
	IdleState->addTransition(service, SIGNAL(stateMachineGetStarted()), GetState);
	IdleState->addTransition(protocol, SIGNAL(disconnected(Account)), OfflineState);
	PutState->addTransition(service, SIGNAL(stateMachinePutFinished()), IdleState);
	PutState->addTransition(service, SIGNAL(stateMachinePutFailed()), IdleState);
	PutState->addTransition(protocol, SIGNAL(disconnected(Account)), OfflineState);
	GetState->addTransition(service, SIGNAL(stateMachineGetFinished()), IdleState);
	GetState->addTransition(service, SIGNAL(stateMachineGetFailed()), IdleState);
	GetState->addTransition(protocol, SIGNAL(disconnected(Account)), OfflineState);

	LocalState = new QState(globalState);
	LocalCleanState = new QState(LocalState);
	LocalDirtyState = new QState(LocalState);
	LocalFailedState = new QState(LocalState);

	LocalCleanState->addTransition(service, SIGNAL(stateMachineLocalDirty()), LocalDirtyState);
	LocalDirtyState->addTransition(service, SIGNAL(stateMachinePutFinished()), LocalCleanState);
	LocalDirtyState->addTransition(service, SIGNAL(stateMachinePutFailed()), LocalFailedState);
	LocalFailedState->addTransition(service, SIGNAL(stateMachineLocalDirty()), LocalDirtyState);
	LocalFailedState->addTransition(protocol, SIGNAL(connected(Account)), LocalDirtyState);
	LocalFailedState->addTransition(protocol, SIGNAL(disconnected(Account)), LocalDirtyState);

	RemoteState = new QState(globalState);
	RemoteCleanState = new QState(RemoteState);
	RemoteDirtyState = new QState(RemoteState);
	RemoteFailedState = new QState(RemoteState);

	RemoteCleanState->addTransition(service, SIGNAL(stateMachineRemoteDirty()), RemoteDirtyState);
	RemoteDirtyState->addTransition(service, SIGNAL(stateMachineGetFinished()), RemoteCleanState);
	RemoteDirtyState->addTransition(service, SIGNAL(stateMachineGetFailed()), RemoteFailedState);
	RemoteFailedState->addTransition(service, SIGNAL(stateMachineRemoteDirty()), RemoteDirtyState);
	RemoteFailedState->addTransition(protocol, SIGNAL(connected(Account)), RemoteDirtyState);
	RemoteFailedState->addTransition(protocol, SIGNAL(disconnected(Account)), RemoteDirtyState);

	LocalState->setInitialState(LocalCleanState);
	RemoteState->setInitialState(RemoteDirtyState);
	WorkState->setInitialState(protocol->isConnected() ? IdleState : OfflineState);

	connect(IdleState, SIGNAL(entered()), this, SLOT(checkIfSynchronizationRequired()));
	connect(LocalDirtyState, SIGNAL(entered()), this, SLOT(checkIfSynchronizationRequired()));
	connect(RemoteDirtyState, SIGNAL(entered()), this, SLOT(checkIfSynchronizationRequired()));

	connect(OfflineState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(IdleState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(PutState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(GetState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(LocalCleanState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(LocalDirtyState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(LocalFailedState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(RemoteCleanState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(RemoteDirtyState, SIGNAL(entered()), this, SLOT(printConfiguration()));
	connect(RemoteFailedState, SIGNAL(entered()), this, SLOT(printConfiguration()));

	addState(globalState);

	setInitialState(globalState);
}

GaduContactListStateMachine::~GaduContactListStateMachine()
{
}

void GaduContactListStateMachine::printConfiguration()
{
	QStringList states;

	if (configuration().contains(OfflineState))
		states.append("offline");
	if (configuration().contains(IdleState))
		states.append("idle");
	if (configuration().contains(PutState))
		states.append("put");
	if (configuration().contains(GetState))
		states.append("get");
	if (configuration().contains(LocalCleanState))
		states.append("local-clean");
	if (configuration().contains(LocalDirtyState))
		states.append("local-dirty");
	if (configuration().contains(LocalFailedState))
		states.append("local-failed");
	if (configuration().contains(RemoteCleanState))
		states.append("remote-clean");
	if (configuration().contains(RemoteDirtyState))
		states.append("remote-dirty");
	if (configuration().contains(RemoteFailedState))
		states.append("remote-failed");

	kdebugm(KDEBUG_INFO, "Gadu contact list state machine: [%s]\n", qPrintable(states.join(", ")));
}

void GaduContactListStateMachine::checkIfSynchronizationRequired()
{
	if (shouldPerformGet())
		emit performGet();
	else if (shouldPerformPut())
		emit performPut();
}

bool GaduContactListStateMachine::shouldPerformPut() const
{
	return configuration().contains(IdleState) && configuration().contains(LocalDirtyState) && !configuration().contains(RemoteDirtyState);
}

bool GaduContactListStateMachine::isPerformingPut() const
{
	return configuration().contains(PutState);
}

bool GaduContactListStateMachine::shouldPerformGet() const
{
	return configuration().contains(IdleState) && configuration().contains(RemoteDirtyState);
}

bool GaduContactListStateMachine::isPerformingGet() const
{
	return configuration().contains(GetState);
}

#include "moc_gadu-contact-list-state-machine.cpp"
