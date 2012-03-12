/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef BUDDY_LIST_MODEL_H
#define BUDDY_LIST_MODEL_H

#include <QtCore/QModelIndex>

#include "accounts/accounts-aware-object.h"

#include "buddies/buddy-list.h"
#include "buddies/buddy.h"
#include "buddies/buddy-set.h"
#include "exports.h"
#include "status/status.h"

#include "accounts/accounts-aware-object.h"
#include "model/kadu-abstract-model.h"

class Buddy;

class KADUAPI BuddyListModel : public QAbstractItemModel, public KaduAbstractModel, public AccountsAwareObject
{
	Q_OBJECT

	bool Checkable;
	BuddyList List;
	BuddySet CheckedBuddies;

	Buddy buddyFromVariant(const QVariant &variant) const;
	Contact contactFromVariant(const QVariant &variant) const;

	bool isCheckableIndex(const QModelIndex &index) const;
	Contact buddyContact(const QModelIndex &index, int contactIndex) const;

	void connectBuddy(const Buddy &buddy);
	void disconnectBuddy(const Buddy &buddy);

private slots:
	void buddyUpdated(const Buddy &buddy);
	void buddyStatusChanged(Contact contact, Status oldStatus);
	void contactUpdated(const Contact &contact);

	void contactAboutToBeAdded(const Contact &contact);
	void contactAdded(const Contact &contact);
	void contactAboutToBeRemoved(const Contact &contact);
	void contactRemoved(const Contact &contact);

protected:
	virtual int buddyIndex(const Buddy &buddy) const;
	virtual Buddy buddyAt(int index) const;

public:
	explicit BuddyListModel(QObject *parent = 0);
	virtual ~BuddyListModel();

	void setBuddyList(const BuddyList &list);
	void addBuddy(const Buddy &buddy);
	void removeBuddy(const Buddy &buddy);

	void setCheckable(bool checkable);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
	BuddySet checkedBuddies() const;

	// AbstractContactsModel implementation
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

	// D&D
	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList & indexes) const;

signals:
	void checkedBuddiesChanged(const BuddySet &checkedBuddies);


};

#endif // BUDDY_LIST_MODEL_H
