 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDIES_MODEL_PROXY
#define BUDDIES_MODEL_PROXY

#include <QtGui/QSortFilterProxyModel>

#include "abstract-buddies-model.h"
#include "buddies/filter/abstract-buddy-filter.h"

class BuddiesModelProxy : public QSortFilterProxyModel, public AbstractBuddiesModel
{
	Q_OBJECT

	AbstractBuddiesModel *SourceBuddyModel;
	QList<AbstractBuddyFilter *> Filters;

	bool BrokenStringCompare;
	bool SortByStatus;
	int compareNames(QString n1, QString n2) const;
	
private slots:
	void modelDestroyed();;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	BuddiesModelProxy(QObject *parent = 0);

	virtual void setSourceModel(QAbstractItemModel *sourceModel);
	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	void setSortByStatus(bool sortByStatus);

	// AbstractBuddiesModel implementation
	virtual Buddy buddyAt(const QModelIndex &index) const;
	virtual const QModelIndex buddyIndex(Buddy buddy) const;

};

#endif // BUDDIES_MODEL_PROXY
