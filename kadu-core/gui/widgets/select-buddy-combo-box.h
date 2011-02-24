/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef SELECT_BUDDY_COMBO_BOX_H
#define SELECT_BUDDY_COMBO_BOX_H

#include "buddies/buddy.h"
#include "gui/widgets/kadu-combo-box.h"
#include "exports.h"

class AbstractBuddyFilter;
class SelectBuddyPopup;

class KADUAPI SelectBuddyComboBox : public KaduComboBox<Buddy>
{
	Q_OBJECT

	SelectBuddyPopup *Popup;

private slots:
	void currentIndexChangedSlot(int index);
	void updateValueBeforeChange();
	void rowsRemoved(const QModelIndex &parent, int start, int end);

protected:
	virtual int preferredDataRole() const;
	virtual QString selectString() const;

	virtual void showPopup();
	virtual void hidePopup();

public:
	explicit SelectBuddyComboBox(QWidget *parent = 0);
	virtual ~SelectBuddyComboBox();

	Buddy currentBuddy();

	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

public slots:
	void setCurrentBuddy(Buddy buddy);

signals:
	void buddyChanged(Buddy buddy);

};

#endif // SELECT_BUDDY_COMBO_BOX_H
