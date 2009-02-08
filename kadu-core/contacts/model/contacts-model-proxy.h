 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_MODEL_PROXY
#define CONTACTS_MODEL_PROXY

#include <QtGui/QSortFilterProxyModel>

#include "abstract-contacts-model.h"
#include "filter/abstract-contact-filter.h"

class ContactsModelProxy : public QSortFilterProxyModel, public AbstractContactsModel
{
	Q_OBJECT

	AbstractContactsModel *SourceContactModel;
	QList<AbstractContactFilter *> Filters;

	bool BrokenStringCompare;
	int compareNames(QString n1, QString n2) const;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	ContactsModelProxy(QObject *parent = 0);

	virtual void setSourceModel(QAbstractItemModel* sourceModel);
	void addFilter(AbstractContactFilter *filter);
	void removeFilter(AbstractContactFilter *filter);

	// IContactsModel implementation
	virtual const QModelIndex contactIndex(Contact contact) const;

};

#endif // CONTACTS_MODEL_PROXY
