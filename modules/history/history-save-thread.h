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
	bool Enabled;

	QMutex SomethingToSave;
	QWaitCondition WaitForSomethingToSave;

	QDateTime LastSyncTime;

	bool Stopped;

	void storeMessages();
	void storeStatusChanges();
	void sync();

public:
	explicit HistorySaveThread(History *history, QObject *parent = 0);
	virtual ~HistorySaveThread();

	virtual void run();

	void forceSync();

	void setEnabled(bool enabled);

	void newDataAvailable();
	void stop();

};

#endif // HISTORY_SAVE_THREAD_H
