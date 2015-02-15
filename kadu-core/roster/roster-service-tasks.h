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

#include "roster/roster-task.h"
#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QVector>

class Contact;

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @class RosterServiceTasks
 * @short Stores list of tasks that roster service should execute when connection is available.
 *
 * For tasks-bases rosters (as in XMPP), if an action cannot be executed immediately it is stored as
 * @link RosterTask @endlink object for later execution. List of tasks is available by calling tasks() getter and can be
 * update by addTasks(). This allows for storing and restoring this list between program invocations. Only one
 * @link RosterTask @endlink for each contact id can be on the list at a time. This class is responsible of choosing which task
 * should be left on the list and which one should be removed in case when second one is added for given id.
 *
 * When receiving updates from the remote roster, list of tasks is checked for id of contact from remote roster. Requests for
 * contact removal or update from remote roster will be ignored if an update or addition task is on the task list. This service
 * assumes that its changes are more important that these of remote roster. Update requests are ignored if Detached flag
 * of @link RosterEntry @endlink of given contact is set to true.
 */
class KADUAPI RosterServiceTasks : public QObject
{
	Q_OBJECT

public:
	/**
	 * @return List of tasks required to update @p contacts
	 *
	 * For each contact that has a RosterEntry and is in desynchronized state a new RosterTask with Update mode is returned.
	 */
	static QVector<RosterTask> updateTasksForContacts(const QVector<Contact> &contacts);

	explicit RosterServiceTasks(QObject *parent = nullptr);
	virtual ~RosterServiceTasks();

	/**
	 * @return Current set of tasks to execute.
	 *
	 * Can be used to store for later execution in external storage.
	 */
	QVector<RosterTask> tasks();

	/**
	 * @short Add new task for later execution.
	 * @param task new task
	 *
	 * If existing task for given id is available then this service decides which one to use and which one to ignore.
	 * If existing task is deletion, then it is always replaced. Addition task can be only replaced by deletion task. Update task
	 * can be replaced by any non-update task.
	 */
	void addTask(const RosterTask &task);

	/**
	 * @short Add new tasks for later execution.
	 * @param tasks new tasks
	 *
	 * Can be used to restore tasks saved in external storage.
	 */
	void addTasks(const QVector<RosterTask> &tasks);

	/**
	 * @return type of task for given id
	 *
	 * If no task for given id is found then RosterTaskType::None is returned.
	 */
	RosterTaskType taskType(const QString &id);

	/**
	 * @return true, if no tasks are held
	 */
	bool isEmpty() const;

	/**
	 * @return first task from queue
	 * @pre !isEmpty()
	 */
	RosterTask dequeue();

	/**
	 * @return true if task with given id is on the list
	 */
	bool containsTask(const QString &id) const;

private:
	QQueue<RosterTask> m_tasks;
	QMap<QString, RosterTask> m_idToTask;

	/**
	 * @return true if task of second type should replace task of first type on list of task to execute
	 */
	bool shouldReplaceTask(RosterTaskType taskType, RosterTaskType replacementType);

};

/**
 * @}
 */
