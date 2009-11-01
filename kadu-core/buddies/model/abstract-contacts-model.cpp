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

#include "abstract-contacts-model.h"

Contact AbstractContactsModel::contact(const QModelIndex &index) const
{
	QVariant conVariant = index.data(ContactRole);
	if (!conVariant.canConvert<Contact>())
		return Contact::null;

	return conVariant.value<Contact>();
}
