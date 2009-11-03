/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SELECT_BUDDY_COMBOBOX_H
#define SELECT_BUDDY_COMBOBOX_H

#include <QtGui/QComboBox>

#include "buddies/buddy.h"

class AbstractBuddyFilter;
class BuddiesModelProxy;
class SelectBuddyPopup;

class SelectBuddyCombobox : public QComboBox
{
	Q_OBJECT

	Buddy MyBuddy;
	BuddiesModelProxy *ProxyModel;
	SelectBuddyPopup *Popup;

private slots:
	void buddyTextChanged(const QString &);

protected:
	virtual void showPopup();
	virtual void hidePopup();

public:
	explicit SelectBuddyCombobox(QWidget *parent = 0);
	virtual ~SelectBuddyCombobox();

	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	Buddy buddy() { return MyBuddy; }

signals:
	void buddyChanged(Buddy buddy);

};

#endif // SELECT_BUDDY_COMBOBOX_H
