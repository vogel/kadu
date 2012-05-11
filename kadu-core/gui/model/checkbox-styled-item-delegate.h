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

#ifndef CHECKBOX_STYLED_ITEM_DELETAGE_H
#define CHECKBOX_STYLED_ITEM_DELETAGE_H

#include <QtGui/QStyledItemDelegate>

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class CheckboxStyledItemDelegate
 * @author Rafał 'Vogel' Malinowski
 * @short Delegate for item view that use checkboxes.
 *
 * This class can be used in any item view to diplay checkbox for a boolean value both for viewing and editing
 * model data.
 */
class CheckboxStyledItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return rect for QCheckBox that is centered in containerRect.
	 * @param option item view option, it contains container rect data
	 * @return centered rect for QCheckBox
	 */
	QRect getCenteredComboBoxRect(const QStyleOptionViewItem &option) const;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of delegate with given parent.
	 */
	explicit CheckboxStyledItemDelegate(QObject *parent = 0);
	virtual ~CheckboxStyledItemDelegate();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Paint checkbox with proper checked value read from index's data.
	 * @param painter painter to use
	 * @param option style options
	 * @param index data source
	 */
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new editor - QCheckBox instance.
	 * @param parent parent of new widget
	 * @param option style options for new widget, ignored
	 * @param index data source
	 * @return new editor - QCheckBox instance
	 */
	virtual QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Center editor QCheckBox.
	 * @param editor checkbox to center
	 * @param option style options for new widget
	 * @param index data source, ignored
	 */
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Update data on provided QCheckBox.
	 * @param editor widget to udpate, must be QCheckBox
	 * @param index data source
	 */
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Update model data from QCheckBox.
	 * @param editor widget to get new data from, must be QCheckBox
	 * @param model model to update
	 * @param index index to update
	 */
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

/**
 * @}
 */

#endif // CHECKBOX_STYLED_ITEM_DELETAGE_H
