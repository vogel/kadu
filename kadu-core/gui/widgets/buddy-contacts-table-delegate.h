/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_CONTACTS_TABLE_DELEGATE_H
#define BUDDY_CONTACTS_TABLE_DELEGATE_H

#include <QtGui/QStyledItemDelegate>

class BuddyContactsTableDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit BuddyContactsTableDelegate(QObject *parent = 0);
	virtual ~BuddyContactsTableDelegate();

	virtual QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // BUDDY_CONTACTS_TABLE_DELEGATE_H

