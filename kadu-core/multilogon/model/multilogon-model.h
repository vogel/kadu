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

#ifndef MULTILOGON_MODEL_H
#define MULTILOGON_MODEL_H

#include <QtCore/QAbstractTableModel>

class MultilogonService;
class MultilogonSession;

class MultilogonModel : public QAbstractTableModel
{
	Q_OBJECT

	MultilogonService *Service;

private slots:
	void multilogonSessionAboutToBeConnected(MultilogonSession *session);
	void multilogonSessionConnected(MultilogonSession *session);
	void multilogonSessionAboutToBeDisconnected(MultilogonSession *session);
	void multilogonSessionDisconnected(MultilogonSession *session);

public:
	explicit MultilogonModel(MultilogonService *service, QObject *parent);
	virtual ~MultilogonModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

};

#endif // MULTILOGON_MODEL_H
