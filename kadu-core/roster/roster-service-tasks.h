/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
 * @addtogroup Protocol
 * @{
 */

/**
 * @class RosterServiceTasks
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
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return true if task of second type should replace task of first type on list of task to execute.
	 * @return true if task of second type should replace task of first type on list of task to execute
	 */
	bool shouldReplaceTask(RosterTaskType taskType, RosterTaskType replacementType);

};

/**
 * @}
 */
