/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HISTORY_SAVE_THREAD_H
#define HISTORY_SAVE_THREAD_H

#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

class History;

class HistorySaveThread : public QThread
{
	Q_OBJECT

	History *CurrentHistory;

	QMutex SomethingToSave;
	QWaitCondition WaitForSomethingToSave;

	QDateTime LastSyncTime;

	bool Stopped;

public:
	explicit HistorySaveThread(History *history, QObject *parent = 0);
	virtual ~HistorySaveThread();

	virtual void run();

	void newMessagesAvailable();
	void stop();

};

#endif // HISTORY_SAVE_THREAD_H
