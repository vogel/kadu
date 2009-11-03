 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QAbstractProxyModel>

#include "buddies/buddy.h"
#include "model/roles.h"

#include "abstract-buddies-model.h"

Buddy AbstractBuddiesModel::buddyAt(const QModelIndex &index) const
{
	QVariant conVariant = index.data(BuddyRole);
	if (!conVariant.canConvert<Buddy>())
		return Buddy::null;

	return conVariant.value<Buddy>();
}
