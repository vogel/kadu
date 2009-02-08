/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_TAB_BAR_H
#define GROUP_TAB_BAR_H

#include <QtGui/QTabBar>

class GroupContactFilter;
class Group;

class GroupTabBar : public QTabBar
{
	Q_OBJECT

	GroupContactFilter *Filter;

//	QString getNewGroupNameFromUser(bool *ok);

// private slots:
// 	void addToGroup();
// 	void moveToGroup();
/*
protected:
	void dragEnterEvent(QDragEnterEvent* e);
	void dropEvent(QDropEvent *e);*/

public:
	GroupTabBar(QWidget *parent = 0);

	GroupContactFilter * filter() { return Filter; }

private slots:
	void currentChangedSlot(int index);

signals:
	void currentGroupChanged(Group *group);

};

#endif // GROUP_TAB_BAR_H
