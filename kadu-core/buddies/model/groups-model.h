 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUPS_MODEL
#define GROUPS_MODEL

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

class Group;

class GroupsModel : public QAbstractListModel
{
	Q_OBJECT

private slots:
	void groupAboutToBeAdded(Group *group);
	void groupAdded(Group *group);
	void groupAboutToBeRemoved(Group *group);
	void groupRemoved(Group *group);

public:
	GroupsModel(QObject *parent = 0);
	virtual ~GroupsModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Group * group(const QModelIndex &index) const;
	int groupIndex(Group *group);
	QModelIndex groupModelIndex(Group *group);

};

#endif // GROUPS_MODEL
