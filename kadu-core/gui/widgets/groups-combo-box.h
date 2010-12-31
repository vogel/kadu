/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "buddies/group.h"
#include "gui/widgets/kadu-combo-box.h"

class GroupsComboBox : public KaduComboBox<Group>
{
	Q_OBJECT

	bool InActivatedSlot;

	QAction *CreateNewGroupAction;

private slots:
	void resetComboBox();
	void activatedSlot(int index);
	void currentIndexChangedSlot(int index);
	void updateValueBeforeChange();
	void rowsRemoved(const QModelIndex &parent, int start, int end);

protected:
	virtual int preferredDataRole() const;
	virtual QString selectString() const;

public:
	explicit GroupsComboBox(QWidget *parent = 0);
	virtual ~GroupsComboBox();

	void setCurrentGroup(Group group);
	Group currentGroup();

};

#endif // GROUPS_COMBO_BOX_H
