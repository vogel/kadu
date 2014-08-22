/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

#include "protocols-model.h"

ProtocolsModel::ProtocolsModel(QObject *parent) :
		QAbstractListModel(parent)
{
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryAboutToBeRegistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryAboutToBeRegistered(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryRegistered(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryAboutToBeUnregistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryAboutToBeUnregistered(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryUnregistered(ProtocolFactory *)));
}

ProtocolsModel::~ProtocolsModel()
{
}

int ProtocolsModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : ProtocolsManager::instance()->protocolFactories().count();
}

QVariant ProtocolsModel::data(const QModelIndex &index, int role) const
{
	ProtocolFactory *pf = protocolFactory(index);
	if (0 == pf)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return pf->displayName();
		case Qt::DecorationRole:
			return pf->icon().icon();
		case ProtocolRole:
			return QVariant::fromValue<ProtocolFactory *>(pf);
		default:
			return QVariant();
	}
}

ProtocolFactory * ProtocolsModel::protocolFactory(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if (index.row() >= rowCount())
		return 0;

	return ProtocolsManager::instance()->byIndex(index.row());
}

int ProtocolsModel::protocolFactoryIndex(ProtocolFactory *protocolFactory) const
{
	return ProtocolsManager::instance()->indexOf(protocolFactory);
}

QModelIndexList ProtocolsModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	int i = protocolFactoryIndex(value.value<ProtocolFactory *>());
	if (-1 != i)
		result.append(index(i, 0));

	return result;
}

void ProtocolsModel::protocolFactoryAboutToBeRegistered(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void ProtocolsModel::protocolFactoryRegistered(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	endInsertRows();
}

void ProtocolsModel::protocolFactoryAboutToBeUnregistered(ProtocolFactory *protocolFactory)
{
	int index = protocolFactoryIndex(protocolFactory);
	beginRemoveRows(QModelIndex(), index, index);
}

void ProtocolsModel::protocolFactoryUnregistered(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	endRemoveRows();
}

#include "moc_protocols-model.cpp"
