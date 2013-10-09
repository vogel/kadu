/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "multilogon/multilogon-session.h"
#include "protocols/services/multilogon-service.h"

#include <model/roles.h>
#include "multilogon-model.h"

MultilogonModel::MultilogonModel(MultilogonService *service, QObject *parent) :
		QAbstractTableModel(parent), Service(service)
{
	if (Service)
	{
		connect(Service, SIGNAL(multilogonSessionAboutToBeConnected(MultilogonSession*)),
				this, SLOT(multilogonSessionAboutToBeConnected(MultilogonSession*)));
		connect(Service, SIGNAL(multilogonSessionConnected(MultilogonSession*)),
				this, SLOT(multilogonSessionConnected(MultilogonSession*)));
		connect(Service, SIGNAL(multilogonSessionAboutToBeDisconnected(MultilogonSession*)),
				this, SLOT(multilogonSessionAboutToBeDisconnected(MultilogonSession*)));
		connect(Service, SIGNAL(multilogonSessionDisconnected(MultilogonSession*)),
				this, SLOT(multilogonSessionDisconnected(MultilogonSession*)));
	}
}

MultilogonModel::~MultilogonModel()
{
}

int MultilogonModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() || !Service
			? 0
			: Service->sessions().count();
}

int MultilogonModel::columnCount(const QModelIndex& parent) const
{
	return parent.isValid()
			? 0
			: 3;
}

QVariant MultilogonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (Qt::Horizontal != orientation || Qt::DisplayRole != role)
		return QVariant();

	switch (section)
	{
		case 0:
			return tr("Name");
		case 1:
			return tr("Ip");
		case 2:
			return tr("Logon time");
	}

	return QVariant();
}

QVariant MultilogonModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().isValid() || !Service)
		return QVariant();

	int row = index.row();
	if (row < 0 || row >= Service->sessions().count())
		return QVariant();

	MultilogonSession *session = Service->sessions().at(row);
	if (!session)
		return QVariant();

	if (role == MultilogonSessionRole)
		return QVariant::fromValue(session);

	if (Qt::DisplayRole != role)
		return QVariant();

	switch (index.column())
	{
		case 0:
			return session->name();
		case 1:
			return session->remoteAddress().toString();
		case 2:
			return session->logonTime();
	}

	return QVariant();
}

void MultilogonModel::multilogonSessionAboutToBeConnected(MultilogonSession *session)
{
	Q_UNUSED(session)

	int row = rowCount();
	beginInsertRows(QModelIndex(), row, row);
}

void MultilogonModel::multilogonSessionConnected(MultilogonSession *session)
{
	Q_UNUSED(session)

	endInsertRows();
}

void MultilogonModel::multilogonSessionAboutToBeDisconnected(MultilogonSession *session)
{
	int row = Service->sessions().indexOf(session);
	if (-1 == row)
		return;

	beginRemoveRows(QModelIndex(), row, row);
}

void MultilogonModel::multilogonSessionDisconnected(MultilogonSession *session)
{
	Q_UNUSED(session)

	endRemoveRows();
}

#include "moc_multilogon-model.cpp"
