 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_PROPERTIES_WINDOW
#define GROUP_PROPERTIES_WINDOW

#include <QtGui/QWidget>

#include "buddies/group.h"

class QCheckBox;
class QLabel;
class QPushButton;

class GroupPropertiesWindow : public QWidget
{
	Q_OBJECT

	Group group;

	QCheckBox *notifyCheckBox;
	QCheckBox *offlineCheckBox;
	QCheckBox *allGroupCheckBox;
	QCheckBox *iconCheckBox;
	QCheckBox *nameCheckBox;

	QLabel *info;

	QPushButton *icon;
	QString iconPath;

private slots:
	void applyClicked();
	void okClicked();	
	void selectIcon();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	GroupPropertiesWindow(Group editedGroup, QWidget *parent = 0);

};

#endif // GROUP_PROPERTIES_WINDOW
