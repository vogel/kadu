/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QComboBox>

#include "gui/widgets/accounts-combo-box.h"

#include "buddy-contacts-table-delegate.h"

BuddyContactsTableDelegate::BuddyContactsTableDelegate(QObject *parent) :
		QStyledItemDelegate(parent)
{
}

BuddyContactsTableDelegate::~BuddyContactsTableDelegate()
{
}

QWidget * BuddyContactsTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (1 != index.column()) // not account
		return QStyledItemDelegate::createEditor(parent, option, index);

	return new AccountsComboBox(false, parent);
}
