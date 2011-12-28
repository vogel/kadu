/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHECKABLE_BUDDIES_PROXY_MODEL_H
#define CHECKABLE_BUDDIES_PROXY_MODEL_H

#include <QtGui/QIdentityProxyModel>

#include "buddies/buddy-set.h"

/**
 * @addtogroup Buddy
 * @{
 */

/**
 * @class Buddy
 * @author Rafał 'Vogel' Malinowski
 * @short Proxy models that adds check capabilities to any model with buddies.
 *
 * Use this proxy model to add check capabilities to any model with buddies. Use will be able to use checkboxed
 * to select any group of buddies. Checked buddies can be retreived using checkedBuddies() getter.
 *
 * Note that buddies are not removed from this list even if they are removed from source model. This allows to avoid
 * clearing this list by short term filtering (like filtering by name to check other buddies).
 */
class CheckableBuddiesProxyModel : public QIdentityProxyModel
{
	Q_OBJECT

	BuddySet CheckedBuddies;

public:
	explicit CheckableBuddiesProxyModel(QObject *parent = 0);
	virtual ~CheckableBuddiesProxyModel();

	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns set of checked buddies.
	 * @return set of checked buddies
	 */
	BuddySet checkedBuddies() const;

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when set of checked buddies changes.
	 */
	void checkedBuddiesChanged(const BuddySet &checkedBuddies);

};

/**
 * @}
 */

#endif // CHECKABLE_BUDDIES_PROXY_MODEL_H
