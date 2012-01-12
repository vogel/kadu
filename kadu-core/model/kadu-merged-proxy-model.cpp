/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "kadu-merged-proxy-model.h"

KaduMergedProxyModel::KaduMergedProxyModel(QObject *parent) :
		MergedProxyModel(parent)
{
}

KaduMergedProxyModel::~KaduMergedProxyModel()
{
}

void KaduMergedProxyModel::setModels(QList<QAbstractItemModel *> models)
{
	Models.clear();
	foreach (QAbstractItemModel *model, models)
	{
		KaduAbstractModel *kaduModel = dynamic_cast<KaduAbstractModel *>(model);
		Q_ASSERT(kaduModel);

		Models.append(kaduModel);
	}

	MergedProxyModel::setModels(models);
}

QModelIndexList KaduMergedProxyModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;
	foreach (KaduAbstractModel *kaduModel, Models)
	{
		const QModelIndexList &kaduModelIndexes = kaduModel->indexListForValue(value);
		foreach (const QModelIndex &index, kaduModelIndexes)
			result.append(mapFromSource(index));
	}

	return result;
}
