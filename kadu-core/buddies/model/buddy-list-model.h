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

#include "status/status.h"

#include "buddies-model-base.h"

class Buddy;

class BuddyListModel : public BuddiesModelBase
{
	Q_OBJECT

	BuddyList List;

protected:
	virtual int buddyIndex(const Buddy &buddy) const;
	virtual Buddy buddyAt(int index) const;

public:
	explicit BuddyListModel(QObject *parent = 0);
	virtual ~BuddyListModel();

	void setBuddyList(const BuddyList &list);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

};

#endif // BUDDY_LIST_MODEL_H
