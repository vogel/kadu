/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef MERGED_PROXY_MODEL_H
#define MERGED_PROXY_MODEL_H

#include <QtCore/QAbstractItemModel>

/**
 * @addtogroup Model
 * @{
 */

/**
 * @class MergedProxyModel
 * @author Rafał 'Vogel' Malinowski
 * @short Model that merges other models into one.
 *
 * Model that merges other models into one. This class merges models one after another, by rows.
 * It does not support columns (number of columns is forced to one). Models are set using setModels()
 * method.
 *
 * The most tricky part of its implementation is computing of parents of mapped indexes. As QModelIndex
 * class does not really store anything we must keep mapping from proxy index to source parent index
 * in separate struct named Mapping that is stored in proxy indexes' internalPointer field.
 */
class MergedProxyModel : public QAbstractItemModel
{
	Q_OBJECT

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Mapping between source indexes and Mapping instances with these indexes.
	 *
	 * This type maps QModelIndex to pointer that contain this index. It is used
	 * to cache these pointers and to allow to remove them if not needed anymore.
	 *
	 * Storing QModelIndex * values in source indexes' internalPointer is not possible, as we can
	 * only alter proxy indexes' internalPointer values.
	 */
	typedef QHash<QModelIndex, QModelIndex *> IndexMapping;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Mapping between source models and their boundaries in merged model.
	 *
	 * This type maps QModelIndex source models and their boundaries in merged model.
	 * Values consists of two integers - first contains first row index of model in merged
	 * model and second contans last row index of model in merged model. If no rows are available
	 * second value is less than first.
	 *
	 * Values of this type must be updated every time count of rows in root index in any of source
	 * models changes.
	 */
	typedef QMap<const QAbstractItemModel *, QPair<int, int> > ModelMap;

	mutable IndexMapping Mappings;
	mutable ModelMap Boundaries;

	QList<QAbstractItemModel *> Models;

	QModelIndexList IndexesToRemove;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Maps proxy index to source index at root level.
	 * @param proxyIndex proxy index at root level
	 * @return source index corresponding to proxyIndex
	 *
	 * This method maps proxy index to source index at root level. Selection of source model is based
	 * on Boundaries field that is compared with proxyIndex row. Then proxyIndex row value is adjusted
	 * and new index from source model is returned.
	 */
	QModelIndex mapFirstLevelToSource(const QModelIndex &proxyIndex) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Maps source index to proxy index at root level.
	 * @param sourceIndex source index at root level
	 * @return proxy index corresponding to sourceIndex
	 *
	 * This method maps source index to proxy index at root level. SourceIndex row value is adjusted
	 * based on Boundaries value mapped to its model and new index from proxy model is returned.
	 */
	QModelIndex mapFirstLevelFromSource(const QModelIndex &sourceIndex) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new mapping for given index or returns existing one.
	 * @param sourceParent source index to create mapping for
	 * @return new or existing mapping for given index
	 *
	 * This method tries to find existing mapping for given sourceParent in Mappings field. If now
	 * mapping is found it creates new one, adds it to Mappings field and returns it. Created mapping
	 * will contain sourceParent value.
	 */
	QModelIndex * createMapping(const QModelIndex &sourceParent) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Removes mapping for given index and all of its subindexes.
	 * @param sourceParent source index to remove mapping
	 *
	 * This method removed all mappings of sourceParent, its subindexes and subindexes of them.
	 */
	void removeMapping(const QModelIndex &sourceParent) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns source parent of given proxy index from mapping.
	 * @param proxyIndex index to find source parent from
	 * @return source parent index for given proxy index
	 *
	 * This returns source parent of given proxy index from mapping. Source parent for each proxy index
	 * is stored in Mapping instance that is pointed to in proxy indexes' internalPointer field.
	 */
	QModelIndex mappedSourceParent(const QModelIndex &proxyIndex) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates boundaries of all submodels of this model.
	 *
	 * Updates boundaries of all submodels of this model. This method must be called when count of rows
	 * on first level of any of the submodels changes.
	 */
	void updateBoundaries() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Helper method to return row offset of given submodel.
	 * @param model model to return offset for.
	 *
	 * This method returns row offset of given submodel. Offset is equal to first value of boundary
	 * for given model.
	 */
	int modelRowOffset(QAbstractItemModel *model) const;

	void connectModels();
	void disconnectModels();

private slots:
	void dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight);

	void rowsAboutToBeInsertedSlot(const QModelIndex &parent, int first, int last);
	void rowsInsertedSlot(const QModelIndex &parent, int first, int last);

	void rowsAboutToBeRemovedSlot(const QModelIndex &parent, int first, int last);
	void rowsRemovedSlot(const QModelIndex &parent, int first, int last);

	void rowsAboutToBeMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
	void rowsMovedSlot(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

	void modelAboutToBeResetSlot();
	void modelResetSlot();

	void modelDestroyedSlot(QObject *model);

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Maps proxy index to source index.
	 * @param proxyIndex proxy index
	 * @return source index corresponding to proxyIndex
	 *
	 * This method maps proxy index to source index. If proxyIndex has not parent, mapFirstLevelToSource
	 * is used to do the mapping. If it has a parent then its parent is mapped first and then new index
	 * with computed parent and unchanged row/column is created.
	 */
	QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Maps source index to proxy index.
	 * @param sourceIndex source index
	 * @return proxy index corresponding to sourceIndex
	 *
	 * This method maps source index to proxy index. If sourceIndex has not parent, mapFirstLevelFromSource
	 * is used to do the mapping. If it has a parent then its parent is mapped first and then new index
	 * with computed parent and unchanged row/column is created.
	 */
	QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new instance of MergedProxyModel.
	 * @param parent QObject parent of new MergedProxyModel.
	 *
	 * Creates new instance of MergedProxyModel.
	 */
	explicit MergedProxyModel(QObject *parent = 0);
	virtual ~MergedProxyModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets list of models to merge.
	 * @param models list of models to merge
	 *
	 * Sets list of models to merge.
	 */
	void setModels(QList<QAbstractItemModel *> models);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &proxyChild) const;

	virtual int rowCount(const QModelIndex &proxyParent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &proxyParent = QModelIndex()) const;

	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;

	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList &proxyIndexes) const;

};

/**
 * @}
 */

#endif // MERGED_PROXY_MODEL_H
