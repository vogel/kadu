/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
	explicit GroupPropertiesWindow(Group editedGroup, QWidget *parent = 0);

};

#endif // GROUP_PROPERTIES_WINDOW
