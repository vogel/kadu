/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "history.h"

#include "history-save-thread.h"

// 10 seconds
#define SYNCHRONIZATION_TIMEOUT 15*1000

HistorySaveThread::HistorySaveThread(History *history, QObject *parent) :
		QThread(parent), CurrentHistory(history), Stopped(false)
{
}

HistorySaveThread::~HistorySaveThread()
{
}

void HistorySaveThread::run()
{
	LastSyncTime = QDateTime::currentDateTime();

	while (!Stopped)
	{
		SomethingToSave.lock();

		if (CurrentHistory->currentStorage())
		{
			while (Message message = CurrentHistory->dequeueUnsavedMessage())
				CurrentHistory->currentStorage()->appendMessage(message);

			if (QDateTime::currentDateTime().addMSecs(-SYNCHRONIZATION_TIMEOUT) >= LastSyncTime)
			{
				CurrentHistory->currentStorage()->sync();
				LastSyncTime = QDateTime::currentDateTime();
			}
		}

		WaitForSomethingToSave.wait(&SomethingToSave, SYNCHRONIZATION_TIMEOUT);
		SomethingToSave.unlock();
	}

	if (CurrentHistory->currentStorage())
		CurrentHistory->currentStorage()->sync();
}

void HistorySaveThread::newMessagesAvailable()
{
	WaitForSomethingToSave.wakeAll();
}

void HistorySaveThread::stop()
{
	Stopped = true;
	WaitForSomethingToSave.wakeAll();
}
