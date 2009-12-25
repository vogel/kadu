/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QComboBox>

#include "accounts/account.h"
#include "gui/widgets/accounts-combo-box.h"
#include "model/roles.h"

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

void BuddyContactsTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	AccountsComboBox *accountsComboBox = dynamic_cast<AccountsComboBox *>(editor);
	if (!accountsComboBox)
	{
		QStyledItemDelegate::setEditorData(editor, index);
		return;
	}

	accountsComboBox->setCurrentAccount(qvariant_cast<Account>(index.data(AccountRole)));
}

void BuddyContactsTableDelegate::setModelData(QWidget* editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	AccountsComboBox *accountsComboBox = dynamic_cast<AccountsComboBox *>(editor);
	if (!accountsComboBox)
	{
		QStyledItemDelegate::setModelData(editor, model, index);
		return;
	}

	model->setData(index, QVariant::fromValue<Account>(accountsComboBox->currentAccount()), AccountRole);
}
