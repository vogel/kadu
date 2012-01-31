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

#ifndef ACTION_FILTER_PROXY_MODEL_H
#define ACTION_FILTER_PROXY_MODEL_H

#include <QtGui/QSortFilterProxyModel>

class QAction;

class ActionFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	int Size;
	QAbstractItemModel *Model;
	QList<QAction *> HideWhenModelEmpty;
	QList<QAction *> HideWhenModelSingle;

private slots:
	void updateSize();

protected:
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	explicit ActionFilterProxyModel(QObject *parent = 0);
	virtual ~ActionFilterProxyModel();

	void setModel(QAbstractItemModel *model);
	void addHideWhenModelEmpty(QAction *action);
	void addHideWhenModelSingle(QAction *action);

};

#endif // ACTION_FILTER_PROXY_MODEL_H
