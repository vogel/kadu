/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef BUDDIES_MODEL_H
#define BUDDIES_MODEL_H

#include <QtCore/QModelIndex>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "status/status.h"

#include "buddies-model-base.h"

class BuddyManager;

class BuddiesModel : public BuddiesModelBase
{
	Q_OBJECT

	bool IncludeMyself;
	bool DetachingOrAttaching;

private slots:
	void buddyAboutToBeAdded(Buddy &buddy);
	void buddyAdded(Buddy &buddy);
	void buddyAboutToBeRemoved(Buddy &buddy);
	void buddyRemoved(Buddy &buddy);

	void myselfBuddyUpdated();
	void buddyUpdated(Buddy &buddy);

	void contactAboutToBeAttached(Contact contact, Buddy nearFutureBuddy);
	void contactAttached(Contact contact);
	void contactAboutToBeDetached(Contact contact);
	void contactDetached(Contact contact, Buddy previousBuddy);

	void contactUpdated(Contact &contact);

protected:
	virtual int buddyIndex(const Buddy &buddy) const;
	virtual Buddy buddyAt(int index) const;

public:
	explicit BuddiesModel(QObject *parent = 0);
	virtual ~BuddiesModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	void setIncludeMyself(bool includeMyself);
	bool includeMyself() const { return IncludeMyself; }

};

#endif // BUDDIES_MODEL_H
