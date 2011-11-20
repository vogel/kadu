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

#ifndef IDENTITY_MODEL_H
#define IDENTITY_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "model/kadu-abstract-model.h"

class Identity;

class IdentityModel : public QAbstractListModel, public KaduAbstractModel
{
	Q_OBJECT

private slots:
	void identityAboutToBeAdded(Identity identity);
	void identityAdded(Identity identity);
	void identityAboutToBeRemoved(Identity identity);
	void identityRemoved(Identity identity);

public:
	explicit IdentityModel(QObject *parent = 0);
	virtual ~IdentityModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;

	Identity identity(const QModelIndex &index) const;
	int identityIndex(Identity identity) const;
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

};

#include "identities/identity.h" // for MOC

#endif // IDENTITY_MODEL_H
