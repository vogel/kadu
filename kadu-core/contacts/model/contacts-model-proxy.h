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

class ContactsModelProxy : public QSortFilterProxyModel, public AbstractContactsModel
{
	Q_OBJECT

	AbstractContactsModel *SourceContactModel;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public:
	ContactsModelProxy(QObject *parent = 0);

	virtual void setSourceModel(QAbstractItemModel* sourceModel);

	// IContactsModel implementation
	virtual const QModelIndex contactIndex(Contact contact) const;

};

#endif // CONTACTS_MODEL_PROXY
