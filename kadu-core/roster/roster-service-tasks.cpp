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

#include "roster-service-tasks.h"

#include "contacts/contact.h"
#include "roster/roster-entry-state.h"
#include "roster/roster-entry.h"
#include "roster/roster-task-type.h"

QVector<RosterTask> RosterServiceTasks::updateTasksForContacts(const QVector<Contact> &contacts)
{
	auto result = QVector<RosterTask>{};
	for (auto &&contact : contacts)
	{
		if (!contact.rosterEntry())
			continue;
		// add also synchronizing and detached contacts, roster service will take care
		// of whether it should really do the merging
		if (contact.rosterEntry()->state() != RosterEntryState::Synchronized)
			result.append(RosterTask{RosterTaskType::Update, contact.id()});
	}
	return result;
}

RosterServiceTasks::RosterServiceTasks(QObject *parent) :
		QObject{parent}
{
}

RosterServiceTasks::~RosterServiceTasks()
{
}

bool RosterServiceTasks::containsTask(const QString &id) const
{
	return m_idToTask.contains(id);
}

QVector<RosterTask> RosterServiceTasks::tasks()
{
	return m_tasks.toVector();
}

bool RosterServiceTasks::shouldReplaceTask(RosterTaskType taskType, RosterTaskType replacementType)
{
	Q_ASSERT(RosterTaskType::None != taskType);
	Q_ASSERT(RosterTaskType::None != replacementType);

	if (RosterTaskType::Delete == taskType)
		return true;

	if (RosterTaskType::Add == taskType)
		return RosterTaskType::Delete == replacementType;

	return RosterTaskType::Update != replacementType;
}

void RosterServiceTasks::addTask(const RosterTask &task)
{
	if (!m_idToTask.contains(task.id()))
	{
		m_tasks.enqueue(task);
		return;
	}

	auto existingTask = m_idToTask.value(task.id());
	if (shouldReplaceTask(existingTask.type(), task.type()))
	{
		m_tasks.removeAll(existingTask);
		m_idToTask.remove(task.id());
		m_idToTask.insert(task.id(), task);
		m_tasks.enqueue(task);
	}
}

void RosterServiceTasks::addTasks(const QVector<RosterTask> &tasks)
{
	for (auto const &task : tasks)
		addTask(task);
}

RosterTaskType RosterServiceTasks::taskType(const QString &id)
{
	if (!m_idToTask.contains(id))
		return RosterTaskType::None;
	else
		return m_idToTask.value(id).type();
}

bool RosterServiceTasks::isEmpty() const
{
	return m_tasks.isEmpty();
}

RosterTask RosterServiceTasks::dequeue()
{
	Q_ASSERT(!isEmpty());

	auto result = m_tasks.dequeue();
	m_idToTask.remove(result.id());
	return result;
}

#include "moc_roster-service-tasks.cpp"
