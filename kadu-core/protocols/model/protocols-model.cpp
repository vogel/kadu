 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
}

ProtocolsModel::~ProtocolsModel()
{
}

int ProtocolsModel::rowCount(const QModelIndex &parent) const
{
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
			return IconsManager::instance()->loadIcon(pf->iconName());
		case ProtocolRole:
			return pf->name();
		default:
			return QVariant();
	}
}

QVariant ProtocolsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

ProtocolFactory * ProtocolsModel::protocolFactory(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if (index.row() > 0 && index.row() >= rowCount())
		return 0;

	return ProtocolsManager::instance()->byIndex(index.row());
}

int ProtocolsModel::protocolFactoryIndex(ProtocolFactory *protocolFactory)
{
	return ProtocolsManager::instance()->indexOf(protocolFactory);
}
