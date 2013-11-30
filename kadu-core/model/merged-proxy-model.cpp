/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QMimeData>
#include <QtCore/QStringList>

#include "merged-proxy-model.h"

MergedProxyModel::MergedProxyModel(QObject *parent) :
		QAbstractItemModel(parent)
{
	setSupportedDragActions(Qt::LinkAction);
}

MergedProxyModel::~MergedProxyModel()
{
}

void MergedProxyModel::connectModels()
{
	foreach (const QAbstractItemModel *model, Models)
	{
		Q_ASSERT(model);

		connect(model, SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()), Qt::DirectConnection);
		connect(model, SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()), Qt::DirectConnection);

		connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChangedSlot(QModelIndex,QModelIndex)), Qt::DirectConnection);

		connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
		        this, SLOT(rowsAboutToBeInsertedSlot(QModelIndex,int,int)), Qt::DirectConnection);
		connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
		        this, SLOT(rowsInsertedSlot(QModelIndex,int,int)), Qt::DirectConnection);

		connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
		        this, SLOT(rowsAboutToBeRemovedSlot(QModelIndex,int,int)), Qt::DirectConnection);
		connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
		        this, SLOT(rowsRemovedSlot(QModelIndex,int,int)), Qt::DirectConnection);

		connect(model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
		        this, SLOT(rowsAboutToBeMovedSlot(QModelIndex,int,int,QModelIndex,int)), Qt::DirectConnection);
		connect(model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
		        this, SLOT(rowsMovedSlot(QModelIndex,int,int,QModelIndex,int)), Qt::DirectConnection);

		connect(model, SIGNAL(modelAboutToBeReset()), this, SLOT(modelAboutToBeResetSlot()), Qt::DirectConnection);
		connect(model, SIGNAL(modelReset()), this, SLOT(modelResetSlot()), Qt::DirectConnection);

		connect(model, SIGNAL(destroyed(QObject*)), this, SLOT(modelDestroyedSlot(QObject*)), Qt::DirectConnection);
	}
}

void MergedProxyModel::disconnectModels()
{
	foreach (const QAbstractItemModel *model, Models)
	{
		Q_ASSERT(model);

		disconnect(model,0, this, 0);
	}
}

void MergedProxyModel::setModels(QList<QAbstractItemModel *> models)
{
	beginResetModel();

	disconnectModels();
	Models = models;
	connectModels();
	updateBoundaries();

	endResetModel();
}

void MergedProxyModel::updateBoundaries() const
{
	Boundaries.clear();

	int index = 0;
	foreach (const QAbstractItemModel *model, Models)
	{
		Q_ASSERT(model);

		const int count = model->rowCount();
		Boundaries.insert(model, qMakePair(index, index + count - 1));
		index += model->rowCount();
	}
}

int MergedProxyModel::modelRowOffset(QAbstractItemModel *model) const
{
	Q_ASSERT(model);
	Q_ASSERT(Boundaries.contains(model));

	return Boundaries.value(model).first;
}

void MergedProxyModel::dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	const QModelIndex &proxyTopLeft = mapFromSource(topLeft);
	const QModelIndex &proxyBottomRight = mapFromSource(bottomRight);

	emit dataChanged(proxyTopLeft, proxyBottomRight);
}

void MergedProxyModel::rowsAboutToBeInsertedSlot(const QModelIndex &parent, int first, int last)
{
	QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(sender());
	Q_UNUSED(model);
	Q_ASSERT(model);
	Q_ASSERT(Boundaries.contains(model));

	const QModelIndex &proxyParent = mapFromSource(parent);
	const int offset = proxyParent.isValid()
	        ? 0
	        : modelRowOffset(model);

	beginInsertRows(proxyParent, first + offset, last + offset);
}

void MergedProxyModel::rowsInsertedSlot(const QModelIndex &parent, int first, int last)
{
	Q_UNUSED(parent);
	Q_UNUSED(first);
	Q_UNUSED(last);

	QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(sender());
	Q_UNUSED(model);
	Q_ASSERT(model);
	Q_ASSERT(Boundaries.contains(model));

	updateBoundaries();

	endInsertRows();
}

void MergedProxyModel::rowsAboutToBeRemovedSlot(const QModelIndex &parent, int first, int last)
{
	QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(sender());
	Q_ASSERT(model);
	Q_ASSERT(Boundaries.contains(model));

	const QModelIndex &proxyParent = mapFromSource(parent);
	const int offset = proxyParent.isValid()
	        ? 0
	        : modelRowOffset(model);

	beginRemoveRows(proxyParent, first + offset, last + offset);

	for (int i = first; i <= last; i++)
		IndexesToRemove.append(model->index(i, 0, parent));
}

void MergedProxyModel::rowsRemovedSlot(const QModelIndex &parent, int first, int last)
{
	Q_UNUSED(parent);
	Q_UNUSED(first);
	Q_UNUSED(last);

	QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(sender());
	Q_UNUSED(model);
	Q_ASSERT(model);
	Q_ASSERT(Boundaries.contains(model));

	foreach (const QModelIndex &sourceIndex, IndexesToRemove)
		removeMapping(sourceIndex);
	IndexesToRemove.clear();

	updateBoundaries();

	endRemoveRows();
}

void MergedProxyModel::rowsAboutToBeMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	Q_UNUSED(destination);
	Q_UNUSED(row);

	// we do not support moving yet
	Q_ASSERT(false);
}

void MergedProxyModel::rowsMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	Q_UNUSED(destination);
	Q_UNUSED(row);

	// we do not support moving yet
	Q_ASSERT(false);
}

void MergedProxyModel::modelAboutToBeResetSlot()
{
	beginResetModel();
}

void MergedProxyModel::modelResetSlot()
{
	updateBoundaries();

	endResetModel();
}

void MergedProxyModel::modelDestroyedSlot(QObject *model)
{
	Models.removeAll(static_cast<QAbstractItemModel *>(model));
}

QModelIndex MergedProxyModel::mapFirstLevelToSource(const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid()) // invalid maps to invalid
		return QModelIndex();

	Q_ASSERT(this == proxyIndex.model());

	ModelMap::const_iterator i = Boundaries.constBegin();
	ModelMap::const_iterator end = Boundaries.constEnd();

	int row = proxyIndex.row();
	while (i != end)
	{
		if (row >= i.value().first && row <= i.value().second)
			return i.key()->index(row - i.value().first, proxyIndex.column());
		i++;
	}

	Q_ASSERT(false); // we should never be here
	return QModelIndex();
}

QModelIndex MergedProxyModel::mapFirstLevelFromSource(const QModelIndex &sourceIndex) const
{
	Q_ASSERT(Boundaries.contains(sourceIndex.model()));

	return index(Boundaries.value(sourceIndex.model()).first + sourceIndex.row(), sourceIndex.column());
}

QModelIndex MergedProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid()) // invalid maps to invalid
		return QModelIndex();

	Q_ASSERT(this == proxyIndex.model());

	const QModelIndex &proxyParentIndex = parent(proxyIndex);
	if (!proxyParentIndex.isValid())
		return mapFirstLevelToSource(proxyIndex);

	const QModelIndex &sourceParentIndex = mapToSource(proxyParentIndex);
	return sourceParentIndex.model()->index(proxyIndex.row(), proxyIndex.column(), sourceParentIndex);
}

QModelIndex MergedProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid()) // invalid maps to invalid
		return QModelIndex();

	Q_ASSERT(Boundaries.contains(sourceIndex.model()));

	const QModelIndex &sourceParentIndex = sourceIndex.parent();
	if (!sourceParentIndex.isValid())
		return mapFirstLevelFromSource(sourceIndex);

	const QModelIndex &proxyParentIndex = mapFromSource(sourceParentIndex);
	return index(sourceIndex.row(), sourceIndex.column(), proxyParentIndex);
}

QModelIndex * MergedProxyModel::createMapping(const QModelIndex &sourceParent) const
{
	IndexMapping::const_iterator it = Mappings.constFind(sourceParent);
	if (it != Mappings.constEnd())
		return it.value();

	QModelIndex *mapping = new QModelIndex(sourceParent);
	Mappings.insert(sourceParent, mapping);

	return mapping;
}

void MergedProxyModel::removeMapping(const QModelIndex &sourceParent) const
{
	QModelIndex *mapping = Mappings.take(sourceParent);
	if (mapping)
		delete mapping;

	IndexMapping::const_iterator i = Mappings.constBegin();
	IndexMapping::const_iterator end = Mappings.constEnd();

	QModelIndexList indexesToRemove;
	while (i != end)
	{
		if (*i.value() == sourceParent)
			indexesToRemove.append(i.key());
		i++;
	}

	foreach (const QModelIndex &index, indexesToRemove)
		removeMapping(index);
}

QModelIndex MergedProxyModel::mappedSourceParent(const QModelIndex &proxyIndex) const
{
	Q_ASSERT(proxyIndex.isValid());
	Q_ASSERT(proxyIndex.model() == this);

	const void *p = proxyIndex.internalPointer();
	if (!p)
		return QModelIndex();

	const QModelIndex *mapping = static_cast<const QModelIndex *>(p);
	return *mapping;
}

QModelIndex MergedProxyModel::index(int row, int column, const QModelIndex &parent) const
{
	if (row < 0 || column < 0)
		return QModelIndex();

	if (!parent.isValid())
		return createIndex(row, column);

	const QModelIndex &sourceParent = mapToSource(parent); // parent is already mapped
	QModelIndex *mapping = createMapping(sourceParent); // map children for this parent

	if (!mapping) // something went wrong
		return QModelIndex();

	return createIndex(row, column, mapping);
}

QModelIndex MergedProxyModel::parent(const QModelIndex &proxyChild) const
{
	if (!proxyChild.isValid())
		return QModelIndex();

	return mapFromSource(mappedSourceParent(proxyChild));
}

int MergedProxyModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
	{
		Q_ASSERT(parent.model() == this);

		const QModelIndex &sourceParent = mapToSource(parent);
		Q_ASSERT(sourceParent.model());

		return sourceParent.model()->rowCount(sourceParent);
	}

	int count = 0;
	foreach (const QAbstractItemModel *model, Models)
	{
		Q_ASSERT(model);
		count += model->rowCount();
	}

	return count;
}

int MergedProxyModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

QFlags<Qt::ItemFlag> MergedProxyModel::flags(const QModelIndex &index) const
{
	return mapToSource(index).flags();
}

QVariant MergedProxyModel::data(const QModelIndex &index, int role) const
{
	return mapToSource(index).data(role);
}

QStringList MergedProxyModel::mimeTypes() const
{
	QStringList result;
	foreach (const QAbstractItemModel *model, Models)
		result += model->mimeTypes();
	return result;
}

QMimeData * MergedProxyModel::mimeData(const QModelIndexList &proxyIndexes) const
{
	if (proxyIndexes.isEmpty())
		return 0;

	QMap<const QAbstractItemModel *, QModelIndexList> sourceIndexes;
	foreach (const QModelIndex &proxyIndex, proxyIndexes)
	{
		const QModelIndex &sourceIndex = mapToSource(proxyIndex);
		const QAbstractItemModel *sourceModel = sourceIndex.model();
		if (!sourceIndexes.contains(sourceModel))
			sourceIndexes.insert(sourceModel, QModelIndexList());
		sourceIndexes.find(sourceModel)->append(sourceIndex);
	}

	QMimeData *mergedMimeData = new QMimeData();

	QList<const QAbstractItemModel *> sourceModels = sourceIndexes.keys();
	foreach (const QAbstractItemModel *sourceModel, sourceModels)
	{
		QMimeData *sourceMimeData = sourceModel->mimeData(sourceIndexes.value(sourceModel));
		if (!sourceMimeData)
			continue;

		foreach (const QString &sourceMimeDataFormat, sourceMimeData->formats())
			mergedMimeData->setData(sourceMimeDataFormat, sourceMimeData->data(sourceMimeDataFormat));

		delete sourceMimeData;
	}

	return mergedMimeData;
}

#include "moc_merged-proxy-model.cpp"
