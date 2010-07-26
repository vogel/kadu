/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

#include "protocols-model.h"

ProtocolsModel::ProtocolsModel(QObject *parent) :
		QAbstractListModel(parent)
{
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryAboutToBeRegistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryAboutToBeRegisteredSlot(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryRegisteredSlot(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryAboutToBeUnregistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryAboutToBeUnregisteredSlot(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory *)),
		this, SLOT(protocolFactoryUnregisteredSlot(ProtocolFactory *)));
}

ProtocolsModel::~ProtocolsModel()
{
}

int ProtocolsModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return ProtocolsManager::instance()->protocolFactories().count();
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
			return pf->icon();
		case ProtocolRole:
			return QVariant::fromValue<ProtocolFactory *>(pf);
		default:
			return QVariant();
	}
}

QVariant ProtocolsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section)
	Q_UNUSED(orientation)
	Q_UNUSED(role)

	return QVariant();
}

ProtocolFactory * ProtocolsModel::protocolFactory(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if (index.row() >= rowCount())
		return 0;

	return ProtocolsManager::instance()->byIndex(index.row());
}

int ProtocolsModel::protocolFactoryIndex(ProtocolFactory *protocolFactory)
{
	return ProtocolsManager::instance()->indexOf(protocolFactory);
}

QModelIndex ProtocolsModel::protocolFactoryModelIndex(ProtocolFactory *protocolFactory)
{
	return index(protocolFactoryIndex(protocolFactory), 0);
}

void ProtocolsModel::protocolFactoryAboutToBeRegisteredSlot(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	beginInsertRows(QModelIndex(), rowCount(), rowCount());
}

void ProtocolsModel::protocolFactoryRegisteredSlot(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	endInsertRows();
}

void ProtocolsModel::protocolFactoryAboutToBeUnregisteredSlot(ProtocolFactory *protocolFactory)
{
	beginRemoveRows(QModelIndex(), protocolFactoryIndex(protocolFactory), protocolFactoryIndex(protocolFactory));
}

void ProtocolsModel::protocolFactoryUnregisteredSlot(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)

	endRemoveRows();
}
