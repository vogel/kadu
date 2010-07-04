/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SELECT_BUDDY_COMBOBOX_H
#define SELECT_BUDDY_COMBOBOX_H

#include <QtGui/QComboBox>

#include "buddies/buddy.h"
#include "exports.h"

class AbstractBuddyFilter;
class ActionsProxyModel;
class BuddiesModel;
class BuddiesModelProxy;
class SelectBuddyPopup;

class KADUAPI SelectBuddyCombobox : public QComboBox
{
	Q_OBJECT

	BuddiesModel *Model;
	BuddiesModelProxy *ProxyModel;
	ActionsProxyModel *ActionsModel;
	SelectBuddyPopup *Popup;

private slots:
	void activatedSlot();

protected:
	virtual void showPopup();
	virtual void hidePopup();

public:
	explicit SelectBuddyCombobox(QWidget *parent = 0);
	virtual ~SelectBuddyCombobox();

	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	Buddy buddy();

public slots:
	void setBuddy(Buddy buddy);

signals:
	void buddyChanged(Buddy buddy);

};

#endif // SELECT_BUDDY_COMBOBOX_H
