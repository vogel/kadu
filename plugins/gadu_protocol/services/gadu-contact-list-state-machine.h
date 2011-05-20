/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GADU_CONTACT_LIST_STATE_MACHINE_H
#define GADU_CONTACT_LIST_STATE_MACHINE_H

#include <QtCore/QSignalTransition>
#include <QtCore/QStateMachine>
#include <QtCore/QTimer>

class GaduContactListService;

class GaduContactListStateMachine : public QStateMachine
{
	Q_OBJECT

	GaduContactListService *CurrentService;

	QTimer RetryTimer;

	QState *OfflineState;
	QState *AwaitingServerGetResponseState;
	QState *AwaitingServerPutResponseState;
	QState *InternalErrorState;
	QState *NormalState;

private slots:
	void printConfiguration();

public:
	explicit GaduContactListStateMachine(GaduContactListService *service);
	virtual ~GaduContactListStateMachine();

	bool awaitingServerGetResponse() const;
	bool awaitingServerPutResponse() const;

signals:
	void awaitingServerGetResponseStateEntered();
	void awaitingServerPutResponseStateEntered();

};

#endif // GADU_CONTACT_LIST_STATE_MACHINE_H
