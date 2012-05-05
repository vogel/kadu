/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>

#include "checkbox-styled-item-delegate.h"

CheckboxStyledItemDelegate::CheckboxStyledItemDelegate(QObject *parent) :
		QStyledItemDelegate(parent)
{
}

CheckboxStyledItemDelegate::~CheckboxStyledItemDelegate()
{
}

void CheckboxStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionButton checkBoxOption;
	checkBoxOption.rect = option.rect;
	checkBoxOption.state = QStyle::State_Enabled | (index.data().toBool() ? QStyle::State_On : QStyle::State_Off);

	QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxOption, painter);
}

QWidget * CheckboxStyledItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option);

	QCheckBox *checkBox = new QCheckBox(parent);
	checkBox->setChecked(index.data().toBool());
	return checkBox;
}

void CheckboxStyledItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
	Q_ASSERT(checkBox);

	checkBox->setChecked(index.data().toBool());
}

void CheckboxStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
	Q_ASSERT(checkBox);

	model->setData(index, checkBox->isChecked());
}
