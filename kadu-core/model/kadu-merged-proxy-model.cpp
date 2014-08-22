/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "model/model-chain.h"

#include "kadu-merged-proxy-model.h"

KaduMergedProxyModel::KaduMergedProxyModel(QObject *parent) :
		MergedProxyModel(parent)
{
}

KaduMergedProxyModel::~KaduMergedProxyModel()
{
}

void KaduMergedProxyModel::setKaduModels(QList<KaduAbstractModel *> models)
{
	QList<QAbstractItemModel *> itemModels;

	KaduModels.clear();
	foreach (KaduAbstractModel *model, models)
	{
		KaduModels.append(model);

		QAbstractItemModel *itemModel = dynamic_cast<QAbstractItemModel *>(model);
		ModelChain *modelChain = dynamic_cast<ModelChain *>(model);
		if (itemModel)
			itemModels.append(itemModel);
		else if (modelChain)
			itemModels.append(modelChain->lastModel());

		Q_ASSERT(itemModel || modelChain);
	}

	MergedProxyModel::setModels(itemModels);
}

QModelIndexList KaduMergedProxyModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;
	foreach (KaduAbstractModel *kaduModel, KaduModels)
	{
		const QModelIndexList &kaduModelIndexes = kaduModel->indexListForValue(value);
		foreach (const QModelIndex &index, kaduModelIndexes)
			result.append(mapFromSource(index));
	}

	return result;
}

#include "moc_kadu-merged-proxy-model.cpp"
