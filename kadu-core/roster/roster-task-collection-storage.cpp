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

#include "roster-task-collection-storage.h"

#include "roster/roster-task-type.h"
#include "roster/roster-task.h"
#include "storage/storage-point.h"

#include <QtCore/QVector>

RosterTaskCollectionStorage::RosterTaskCollectionStorage(std::shared_ptr<StoragePoint> storage) :
		m_storage{storage}
{
	Q_ASSERT(m_storage);
	Q_ASSERT(m_storage->storage());
}

QVector<RosterTask> RosterTaskCollectionStorage::loadRosterTasks()
{
	auto result = QVector<RosterTask>{};

	auto rosterTasksNode = m_storage->storage()->getNode(m_storage->point(), "RosterTasks");

	auto rosterTaskNodes = rosterTasksNode.childNodes();
	auto rosterTaskCount = rosterTaskNodes.count();

	for (decltype(rosterTaskCount) i = 0; i < rosterTaskCount; i++)
	{
		auto rosterTaskElement = rosterTaskNodes.at(i).toElement();
		if (rosterTaskElement.isNull() || rosterTaskElement.text().isEmpty())
			continue;

		if (rosterTaskElement.nodeName() == "Add")
			result.append(RosterTask(RosterTaskType::Add, rosterTaskElement.text()));
		else if (rosterTaskElement.nodeName() == "Delete")
			result.append(RosterTask(RosterTaskType::Delete, rosterTaskElement.text()));
		else if (rosterTaskElement.nodeName() == "Update")
			result.append(RosterTask(RosterTaskType::Update, rosterTaskElement.text()));
	}

	return result;
}

void RosterTaskCollectionStorage::storeRosterTasks(const QVector<RosterTask> &tasks)
{
	auto rosterTasksNode = m_storage->storage()->getNode(m_storage->point(), "RosterTasks");

	while (!rosterTasksNode.childNodes().isEmpty())
		rosterTasksNode.removeChild(rosterTasksNode.childNodes().at(0));

	for (auto &&task : tasks)
		switch (task.type())
		{
			case RosterTaskType::Add:
				m_storage->storage()->createTextNode(rosterTasksNode, "Add", task.id());
				break;
			case RosterTaskType::Delete:
				m_storage->storage()->createTextNode(rosterTasksNode, "Delete", task.id());
				break;
			case RosterTaskType::Update:
				m_storage->storage()->createTextNode(rosterTasksNode, "Update", task.id());
				break;
			default:
				break;
		}
}
