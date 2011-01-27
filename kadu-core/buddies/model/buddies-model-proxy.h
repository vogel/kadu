/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef BUDDIES_MODEL_PROXY
#define BUDDIES_MODEL_PROXY

#include <QtGui/QSortFilterProxyModel>

#include "abstract-buddies-model.h"

class AbstractBuddyFilter;
class AbstractContactFilter;

class BuddiesModelProxy : public QSortFilterProxyModel, public AbstractBuddiesModel
{
	Q_OBJECT

	AbstractBuddiesModel *SourceBuddyModel;
	QList<AbstractBuddyFilter *> BuddyFilters;
	QList<AbstractContactFilter *> ContactFilters;

	bool BrokenStringCompare;
	bool SortByStatus;
	int compareNames(QString n1, QString n2) const;

private slots:
	void modelDestroyed();

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	explicit BuddiesModelProxy(QObject *parent = 0);
	virtual ~BuddiesModelProxy();

	virtual void setSourceModel(QAbstractItemModel *sourceModel);

	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	void addFilter(AbstractContactFilter *filter);
	void removeFilter(AbstractContactFilter *filter);

	void setSortByStatus(bool sortByStatus);

	// AbstractBuddiesModel implementation
	virtual Buddy buddyAt(const QModelIndex &index) const;
	virtual QModelIndex indexForValue(const QVariant &value) const;

};

#endif // BUDDIES_MODEL_PROXY
