/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MODEL_WATCHER_H
#define MODEL_WATCHER_H

#include <QtCore/QAbstractItemModel>

class ModelWatcher : public QObject
{
	Q_OBJECT

	QString Name;

	QString indexToString(const QModelIndex &index);

private slots:
	void columnsAboutToBeInserted(const QModelIndex &parent, int start, int end);
	void columnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn);
	void columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
	void columnsInserted(const QModelIndex &parent, int start, int end);
	void columnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn);
	void columnsRemoved(const QModelIndex &parent, int start, int end);
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	void headerDataChanged(Qt::Orientation orientation, int first, int last);
	void layoutAboutToBeChanged();
	void layoutChanged();
	void modelAboutToBeReset();
	void modelReset();
	void rowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
	void rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
	void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
	void rowsInserted(const QModelIndex &parent, int start, int end);
	void rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
	void rowsRemoved(const QModelIndex &parent, int start, int end);

public:
	explicit ModelWatcher(const QString &name, QAbstractItemModel *model);
	virtual ~ModelWatcher();

};

#endif // MODEL_WATCHER_H
