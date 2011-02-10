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

#include "multilogon/multilogon-session.h"
#include "protocols/services/multilogon-service.h"

#include "multilogon-model.h"

MultilogonModel::MultilogonModel(MultilogonService *service, QObject *parent) :
		QAbstractTableModel(parent), Service(service)
{
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
	if (index.parent().isValid() || Qt::DisplayRole != role || !Service)
		return QVariant();

	int row = index.row();
	if (row < 0 || row >= Service->sessions().count())
		return QVariant();

	MultilogonSession *session = Service->sessions().at(row);
	if (!session)
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
