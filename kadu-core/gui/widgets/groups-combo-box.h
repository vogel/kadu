/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef GROUPS_COMBO_BOX_H
#define GROUPS_COMBO_BOX_H

#include <QtGui/QComboBox>

#include "buddies/group.h"

class AbstractGroupFilter;
class GroupsModel;
class QSortFilterProxyModel;
class ActionsProxyModel;

class GroupsComboBox : public QComboBox
{
	Q_OBJECT

	GroupsModel *Model;
	QSortFilterProxyModel *ProxyModel;
	ActionsProxyModel *ActionsModel;

	QAction *CreateNewGroupAction;

	Group CurrentGroup;

private slots:
	void activatedSlot(int index);
	void resetGroup();

public:
	explicit GroupsComboBox(QWidget *parent = 0);
	virtual ~GroupsComboBox();

	void setCurrentGroup(Group group);
	Group currentGroup();

};

#endif // GROUPS_COMBO_BOX_H
