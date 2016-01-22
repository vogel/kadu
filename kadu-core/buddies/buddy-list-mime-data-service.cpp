/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-list-mime-data-service.h"

#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "misc/memory.h"

#include <QtCore/QMimeData>

BuddyListMimeDataService::BuddyListMimeDataService(QObject *parent) :
		QObject{parent},
		m_mimeType{"application/x-kadu-buddy-list"}
{
}

BuddyListMimeDataService::~BuddyListMimeDataService()
{
}

void BuddyListMimeDataService::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

QStringList BuddyListMimeDataService::mimeTypes()
{
	return QStringList{} << m_mimeType;
}

std::unique_ptr<QMimeData> BuddyListMimeDataService::toMimeData(const BuddyList &buddyList)
{
	if (buddyList.isEmpty())
		return nullptr;

	auto mimeData = make_unique<QMimeData>();
	auto buddyListStrings = QStringList{};
	for (auto const &buddy : buddyList)
		buddyListStrings << buddy.uuid().toString();

	mimeData->setData(m_mimeType, buddyListStrings.join(":").toUtf8());
	return mimeData;
}

BuddyList BuddyListMimeDataService::fromMimeData(const QMimeData *mimeData)
{
	auto result = BuddyList{};
	auto buddyListString = QString{mimeData->data(m_mimeType)};
	if (buddyListString.isEmpty())
		return result;

	auto buddyListStrings = buddyListString.split(':');
	for (auto const &buddyListString : buddyListStrings)
	{
		auto buddy = m_buddyManager->byUuid(buddyListString);
		if (!buddy.isNull())
			result << buddy;
	}

	return result;
}

#include "moc_buddy-list-mime-data-service.cpp"
