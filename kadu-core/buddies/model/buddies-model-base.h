/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDIES_MODEL_BASE_H
#define BUDDIES_MODEL_BASE_H

#include <QtCore/QAbstractItemModel>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "buddies/buddy-set.h"
#include "model/kadu-abstract-model.h"
#include "status/status.h"

class QModelIndex;

class BuddiesModelBase : public QAbstractItemModel, public KaduAbstractModel, public AccountsAwareObject
{
	Q_OBJECT

	bool Checkable;
	BuddySet CheckedBuddies;

	bool isCheckableIndex(const QModelIndex &index) const;
	Contact buddyContact(const QModelIndex &index, int accountIndex) const;

private slots:
	void buddyStatusChanged(Contact contact, Status oldStatus);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	virtual int buddyIndex(const Buddy &buddy) const = 0;
	virtual Buddy buddyAt(int index) const = 0;

public:
	explicit BuddiesModelBase(QObject *parent = 0);
	virtual ~BuddiesModelBase();

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

#endif // BUDDIES_MODEL_BASE_H
