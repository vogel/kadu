/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/model/buddies-model.h"
#include "chat/model/chats-model.h"

#include "talkable-model.h"

TalkableModel::TalkableModel(QObject *parent)
		: MergedProxyModel(parent)
{
	Chats = new ChatsModel(this);
	Buddies = new BuddiesModel(this);

	QList<QAbstractItemModel *> models;
	models.append(Chats);
	models.append(Buddies);
	setModels(models);
}

TalkableModel::~TalkableModel()
{
}

QModelIndexList TalkableModel::indexListForValue(const QVariant &value) const
{
	const QModelIndexList &chatsIndexes = Chats->indexListForValue(value);
	const QModelIndexList &buddiesIndexes = Buddies->indexListForValue(value);

	QModelIndexList result;

	foreach (const QModelIndex &index, chatsIndexes)
		result.append(mapFromSource(index));

	foreach (const QModelIndex &index, buddiesIndexes)
		result.append(mapFromSource(index));

	return result;
}

void TalkableModel::setIncludeMyself(bool includeMyself)
{
	Buddies->setIncludeMyself(includeMyself);
}

bool TalkableModel::includeMyself() const
{
	return Buddies->includeMyself();
}
