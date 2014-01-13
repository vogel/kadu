/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

QRect CheckboxStyledItemDelegate::getCenteredComboBoxRect(const QStyleOptionViewItem &option) const
{
	QSize size = QApplication::style()->sizeFromContents(QStyle::CT_CheckBox, &option, QSize());
#ifdef Q_OS_WIN32
	size.setWidth(size.width() + QApplication::style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option));
#endif

	QPoint center = option.rect.center();
	QRect result;
	result.setTop(center.y() - size.height() / 2);
	result.setLeft(center.x() - size.width() / 2);
	result.setSize(size);

	return result;
}
#include <stdio.h>
void CheckboxStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);
	opt.text.clear(); // do not display anything in background, only show checkbox

	QStyleOptionButton checkBoxOption;

	checkBoxOption.rect = getCenteredComboBoxRect(option);
	checkBoxOption.state = opt.state | (index.data().toBool() ? QStyle::State_On : QStyle::State_Off);

	if (!index.flags().testFlag(Qt::ItemIsEditable))
		checkBoxOption.state = checkBoxOption.state & ~QStyle::State_Enabled;

	QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter); // draw background
	QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxOption, painter);
}

QWidget * CheckboxStyledItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option);

	QCheckBox *checkBox = new QCheckBox(parent);
	checkBox->setChecked(index.data().toBool());

	if (!index.flags().testFlag(Qt::ItemIsEditable))
		checkBox->setEnabled(false);

	return checkBox;
}

void CheckboxStyledItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index);

	editor->setGeometry(getCenteredComboBoxRect(option));
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

#include "moc_checkbox-styled-item-delegate.cpp"
