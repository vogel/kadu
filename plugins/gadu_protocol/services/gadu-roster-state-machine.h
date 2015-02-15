/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QSignalTransition>
#include <QtCore/QStateMachine>
#include <QtCore/QTimer>

class Protocol;

class GaduRosterService;

class GaduRosterStateMachine : public QStateMachine
{
	Q_OBJECT

public:
	explicit GaduRosterStateMachine(GaduRosterService *service, Protocol *protocol);
	virtual ~GaduRosterStateMachine();

	bool shouldPerformPut() const;
	bool isPerformingPut() const;

	bool shouldPerformGet() const;
	bool isPerformingGet() const;

signals:
	void putRequested();
	void getRequested();

	void performPut();
	void performGet();

private:
	QState *m_workState;
	QState *m_offlineState;
	QState *m_idleState;
	QState *m_putState;
	QState *m_getState;

	QState *m_localState;
	QState *m_localCleanState;
	QState *m_localDirtyState;
	QState *m_localCleaningState;
	QState *m_localCleaningDirtyState;
	QState *m_localFailedState;

	QState *m_remoteState;
	QState *m_remoteCleanState;
	QState *m_remoteDirtyState;
	QState *m_remoteCleaningState;
	QState *m_remoteCleaningDirtyState;
	QState *m_remoteFailedState;

private slots:
	void checkIfSynchronizationRequired();
	void printConfiguration();

};
