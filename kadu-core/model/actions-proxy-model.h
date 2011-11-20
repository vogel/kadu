/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTIONS_PROXY_MODEL_H
#define ACTIONS_PROXY_MODEL_H

#include <QtCore/QList>
#include <QtGui/QAbstractProxyModel>

class QAction;

class ActionsProxyModel : public QAbstractProxyModel
{
	Q_OBJECT

public:
	enum ActionVisibility // flag
	{
		AlwaysVisible = 0x0000,
		// TODO: think of better names
		NotVisibleWithEmptySourceModel = 0x0001,
		NotVisibleWithOneRowSourceModel = 0x0002
	};

private:
	struct ActionWithVisibility
	{
		QAction *Action;
		ActionVisibility Visibility;
		ActionWithVisibility(QAction *action, ActionVisibility visibility) : Action(action), Visibility(visibility) {}
	};

	QList<ActionWithVisibility> BeforeActions;
	QList<ActionWithVisibility> AfterActions;
	QList<QAction *> VisibleBeforeActions;
	QList<QAction *> VisibleAfterActions;

	QAction * actionForIndex(const QModelIndex &index) const;
	void updateVisibleActions(QList<QAction *> &visibleActions, const QList<ActionWithVisibility> &actions, int globalPosition);
	void updateVisibleBeforeActions();
	void updateVisibleAfterActions();

private slots:
	void sourceDataChanged(const QModelIndex &, const QModelIndex &);
	void sourceHeaderDataChanged(Qt::Orientation, int, int);
	void sourceRowsAboutToBeInserted(const QModelIndex &, int, int);
	void sourceRowsInserted(const QModelIndex &, int, int);
	void sourceColumnsAboutToBeInserted(const QModelIndex &, int, int);
	void sourceColumnsInserted(const QModelIndex &, int, int);
	void sourceRowsAboutToBeRemoved(const QModelIndex &, int, int);
	void sourceRowsRemoved(const QModelIndex &, int, int);
	void sourceColumnsAboutToBeRemoved(const QModelIndex &, int, int);
	void sourceColumnsRemoved(const QModelIndex &, int, int);
	void sourceLayoutAboutToBeChanged();
	void sourceLayoutChanged();

public:
	explicit ActionsProxyModel(QObject *parent = 0);
	virtual ~ActionsProxyModel();

	void addBeforeAction(QAction *action, ActionVisibility actionVisibility = AlwaysVisible);
	void addAfterAction(QAction *action, ActionVisibility actionVisibility = AlwaysVisible);

	virtual void setSourceModel(QAbstractItemModel *newSourceModel);

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;

	virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

};

Q_DECLARE_METATYPE(QAction *)

#endif // ACTIONS_PROXY_MODEL_H
