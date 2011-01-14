/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "history.h"

#include "history-save-thread.h"

// 15 seconds
#define SYNCHRONIZATION_TIMEOUT 15*1000

HistorySaveThread::HistorySaveThread(History *history, QObject *parent) :
		QThread(parent), CurrentHistory(history), Enabled(true), Stopped(false)
{
}

HistorySaveThread::~HistorySaveThread()
{
}

void HistorySaveThread::storeMessages()
{
	if (!CurrentHistory->currentStorage())
		return;

	while (Message message = CurrentHistory->dequeueUnsavedMessage())
		CurrentHistory->currentStorage()->appendMessage(message);
}

void HistorySaveThread::storeStatusChanges()
{
	if (!CurrentHistory->currentStorage())
		return;

	while (true)
	{
		QPair<Contact, Status> statusChange = CurrentHistory->dequeueUnsavedStatusChange();
		if (!statusChange.first)
			return;

		CurrentHistory->currentStorage()->appendStatus(statusChange.first, statusChange.second);
	}
}

void HistorySaveThread::sync()
{
	if (CurrentHistory->currentStorage())
	{
		CurrentHistory->currentStorage()->sync();
		LastSyncTime = QDateTime::currentDateTime();
	}
}

void HistorySaveThread::forceSync()
{
	SomethingToSave.lock();

	storeMessages();
	storeStatusChanges();

	sync();

	SomethingToSave.unlock();
}

void HistorySaveThread::run()
{
	LastSyncTime = QDateTime::currentDateTime();

	while (!Stopped)
	{
		SomethingToSave.lock();

		if (Enabled)
		{
			storeMessages();
			storeStatusChanges();
			if (QDateTime::currentDateTime().addMSecs(-SYNCHRONIZATION_TIMEOUT) >= LastSyncTime)
				sync();
		}

		WaitForSomethingToSave.wait(&SomethingToSave, SYNCHRONIZATION_TIMEOUT);
		SomethingToSave.unlock();
	}

	storeMessages();
	storeStatusChanges();
	sync();
}

void HistorySaveThread::setEnabled(bool enabled)
{
	Enabled = enabled;
}

void HistorySaveThread::newDataAvailable()
{
	WaitForSomethingToSave.wakeAll();
}

void HistorySaveThread::stop()
{
	Stopped = true;
	WaitForSomethingToSave.wakeAll();
}
