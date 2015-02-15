/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QEvent>
#include <QtWidgets/QComboBox>

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

	AccountsComboBox *accountsComboBox = new AccountsComboBox(index.data(AccountRole).value<Account>().isNull(),
	                                                          AccountsComboBox::NotVisibleWithOneRowSourceModel,
	                                                          parent);
	// this connect does not work withour Account
	connect(accountsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));

	return accountsComboBox;
}

void BuddyContactsTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index)

	editor->setGeometry(option.rect); // use full rect, does not allow to display image next to combo-box
}

void BuddyContactsTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	AccountsComboBox *accountsComboBox = qobject_cast<AccountsComboBox *>(editor);
	if (!accountsComboBox)
	{
		QStyledItemDelegate::setEditorData(editor, index);
		return;
	}

	accountsComboBox->setCurrentAccount(index.data(AccountRole).value<Account>());
}

void BuddyContactsTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	AccountsComboBox *accountsComboBox = qobject_cast<AccountsComboBox *>(editor);
	if (!accountsComboBox)
	{
		QStyledItemDelegate::setModelData(editor, model, index);
		return;
	}

	model->setData(index, QVariant::fromValue<Account>(accountsComboBox->currentAccount()), AccountRole);
}

void BuddyContactsTableDelegate::dataChanged()
{
	QWidget *editorWidget = qobject_cast<QWidget *>(sender());
	if (editorWidget)
		emit commitData(editorWidget);
}

bool BuddyContactsTableDelegate::eventFilter(QObject *editor, QEvent *event)
{
	bool handled = QStyledItemDelegate::eventFilter(editor, event);

	if (!handled && event->type() == QEvent::KeyPress)
	{
		QWidget *editorWidget = qobject_cast<QWidget *>(editor);
		if (editorWidget)
			// we need to delay it a bit, otherwise it is executed before the event goes to the widget
			// it's exactly how Qt does it
			QMetaObject::invokeMethod(this, "commitData", Qt::QueuedConnection, Q_ARG(QWidget *, editorWidget));
	}

	return handled;
}

#include "moc_buddy-contacts-table-delegate.cpp"
