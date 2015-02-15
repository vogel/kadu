/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QAbstractItemModel>
#include <QtCore/QAbstractProxyModel>

#include "model/kadu-abstract-model.h"

#include "model-chain.h"

ModelChain::ModelChain(QObject *parent) :
		QObject(parent), Model(0), KaduModel(0)
{
}

ModelChain::~ModelChain()
{
}

void ModelChain::setBaseModel(QAbstractItemModel *model)
{
	Model = model;

	if (Model)
	{
		KaduModel = dynamic_cast<KaduAbstractModel *>(model);
		Q_ASSERT(KaduModel);
	}
	else
		KaduModel = nullptr;

	if (!ProxyModels.empty())
		ProxyModels.at(0)->setSourceModel(Model);
}

void ModelChain::addProxyModel(QAbstractProxyModel *proxyModel)
{
	if (!proxyModel)
		return;

	if (ProxyModels.empty())
		proxyModel->setSourceModel(Model);
	else
		proxyModel->setSourceModel(ProxyModels.last());

	ProxyModels.append(proxyModel);
}

QAbstractItemModel * ModelChain::firstModel() const
{
	return Model;
}

QAbstractItemModel * ModelChain::lastModel() const
{
	if (!ProxyModels.empty())
		return ProxyModels.last();

	return Model;
}

QModelIndexList ModelChain::indexListForValue(const QVariant &value) const
{
	if (!Model || !KaduModel)
		return QModelIndexList();

	QModelIndexList indexes = KaduModel->indexListForValue(value);
	QModelIndexList result;

	const int size = indexes.size();
	result.reserve(size);

	for (int i = 0; i < size; i++)
	{
		QModelIndex index = indexes.at(i);
		foreach (QAbstractProxyModel *proxyModel, ProxyModels)
			index = proxyModel->mapFromSource(index);
		result.append(index);
	}

	return result;
}

#include "moc_model-chain.cpp"
