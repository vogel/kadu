/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "model/roles.h"
#include "status/description-manager.h"

#include "description-model.h"

DescriptionModel::DescriptionModel(DescriptionManager *manager) :
		Manager(manager)
{
	connect(Manager, SIGNAL(descriptionAboutToBeAdded(const QString &)),
			this, SLOT(descriptionAboutToBeAdded(const QString &)), Qt::DirectConnection);
	connect(Manager, SIGNAL(descriptionAdded(const QString &)),
			this, SLOT(descriptionAdded(const QString &)), Qt::DirectConnection);
	connect(Manager, SIGNAL(descriptionAboutToBeRemoved(const QString &)),
			this, SLOT(descriptionAboutToBeRemoved(const QString &)), Qt::DirectConnection);
	connect(Manager, SIGNAL(descriptionRemoved(const QString &)),
			this, SLOT(descriptionRemoved(const QString &)), Qt::DirectConnection);
}

DescriptionModel::~DescriptionModel()
{
	disconnect(Manager, 0, this, 0);
}

int DescriptionModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : Manager->content().count();
}

Qt::ItemFlags DescriptionModel::flags(const QModelIndex &index) const
{
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable; // fake
}

QVariant DescriptionModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (0 != index.column())
		return QVariant();

	if (index.row() < 0 || index.row() >= Manager->content().count())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
		{
			QString text = Manager->content().at(index.row());
			return text.replace('\n', " / ");
		}

		case DescriptionRole:
			return Manager->content().at(index.row());

		default:
			return QVariant();
	}
}

void DescriptionModel::descriptionAboutToBeAdded(const QString &description)
{
	Q_UNUSED(description)

	beginInsertRows(QModelIndex(), 0, 0);
}

void DescriptionModel::descriptionAdded(const QString &description)
{
	Q_UNUSED(description)

	endInsertRows();
}

void DescriptionModel::descriptionAboutToBeRemoved(const QString &description)
{
	int index = Manager->content().indexOf(description);
	beginRemoveRows(QModelIndex(), index, index);
}

void DescriptionModel::descriptionRemoved(const QString &description)
{
	Q_UNUSED(description)

	endRemoveRows();
}

#include "moc_description-model.cpp"
