/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMutexLocker>

#include "message/message.h"

#include "history-save-thread.h"
#include "history.h"

// 15 seconds
#define SYNCHRONIZATION_TIMEOUT 15*1000

HistorySaveThread::HistorySaveThread(History *history, QObject *parent) :
		QThread(parent), CurrentHistory(history), Enabled(true), Stopped(false), CurrentlySaving(false), ForceSyncOnce(false)
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

void HistorySaveThread::forceSync(bool crashed)
{
	if (crashed)
	{
		// just sync, using threads won't work after crash
		storeMessages();
		storeStatusChanges();
		sync();

		return;
	}

	if (isRunning())
	{
		ForceSyncOnce = true;
		// It doesn't really guarantee that all new data will be sync'ed
		// now, for the same reason as in newDataAvailable() method.
		// But I don't think we really need that - it would decrease
		// history importing performance even more at the cost of very
		// questionable safety.
		WaitForSomethingToSave.wakeAll();
	}
	else
	{
		QMutexLocker locker(&SomethingToSave);

		storeMessages();
		storeStatusChanges();
		sync();
	}
}

void HistorySaveThread::run()
{
	LastSyncTime = QDateTime::currentDateTime();

	// Solution copied from QWaitCondition docs and adjusted.
	SomethingToSave.lock();
	while (!Stopped)
	{
		CurrentlySaving = true;
		SomethingToSave.unlock();

		if (!Stopped && (Enabled || ForceSyncOnce))
		{
			storeMessages();
			storeStatusChanges();
			if (ForceSyncOnce || QDateTime::currentDateTime().addMSecs(-SYNCHRONIZATION_TIMEOUT) >= LastSyncTime)
			{
				sync();
				ForceSyncOnce = false;
			}
		}

		SomethingToSave.lock();
		CurrentlySaving = false;
		WaitForSomethingToSave.wait(&SomethingToSave, SYNCHRONIZATION_TIMEOUT);
	}
	SomethingToSave.unlock();

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
	// If we are currently sync'ing, it will do nothing and some
	// data may be potentially not dequeued now. But we cannot afford
	// to apply the same solution as in stop() method, which would
	// block the main thread. That data will be dequeued later anyway,
	// even if the app crashes.
	WaitForSomethingToSave.wakeAll();
}

void HistorySaveThread::stop()
{
	// Solution copied from QWaitCondition docs and adjusted.

	Stopped = true;

	SomethingToSave.lock();
	while (CurrentlySaving)
	{
		SomethingToSave.unlock();
		QThread::msleep(200);
		SomethingToSave.lock();
	}
	WaitForSomethingToSave.wakeAll();
	SomethingToSave.unlock();
}

#include "moc_history-save-thread.cpp"
