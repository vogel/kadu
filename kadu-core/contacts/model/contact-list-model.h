/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONTACT_LIST_MODEL_H
#define CONTACT_LIST_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "contacts/contact.h"
#include "model/kadu-abstract-model.h"

class ContactListModel : public QAbstractItemModel, public KaduAbstractModel
{
	Q_OBJECT

	QVector<Contact> List;

private slots:
	void contactUpdated();

public:
	explicit ContactListModel(const QVector<Contact> &list, QObject *parent = 0);
	virtual ~ContactListModel();

	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	// KaduAbstractModel implementation
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

};

#endif // CONTACT_LIST_MODEL_H
