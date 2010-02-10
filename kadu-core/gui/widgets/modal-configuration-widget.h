/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MODAL_CONFIGURATION_WIDGET_H
#define MODAL_CONFIGURATION_WIDGET_H

#include <QtGui/QWidget>

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ModalConfigurationWidget
 * @author Rafal 'Vogel' Malinowski
 * @short Widget that should not be hidden when it contains modified data.
 *
 * This widget allows to edit some data. When data is modified and not saved
 * parent widget should not allow to hide it - it should either force saving
 * data or rejecting it (by apply and cancel methods).
 *
 * Current state of widget can be determined by calling needsSave method -
 * when it returns true, widget cannot be hidden or closed.
 */
class ModalConfigurationWidget : public QWidget
{
	Q_OBJECT

	bool NeedsSave;

protected:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sets new value of needsSave.
	 *
	 * Sets new value of needsSave. When new value is different from
	 * old one, needsSaveChanged signal is emited.
	 */
	void setNeedsSave(bool needsSave);

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Creates new ModalConfigurationWidget.
	 * @param parent parent widget
	 *
	 * Creates new ModalConfigurationWidget.
	 */
	explicit ModalConfigurationWidget(QWidget *parent = 0);
	virtual ~ModalConfigurationWidget();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores widget data.
	 *
	 * This method have to be overloaded in derivered widgets. It stores
	 * all widget data and marks it as 'unmodified'.
	 */
	virtual void apply() = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Resets widget data to oryginal state.
	 *
	 * This method have to be overloaded in derivered widgets. It resets
	 * all widget data to oryginal state and marks it as 'unmodified'.
	 */
	virtual void cancel() = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns true when data in widget has been modified.
	 * @return true when data in widget has been modified
	 *
	 * Returns true when data in widget has been modified.
	 */
	bool needsSave();

signals:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emited when state of data modifiecation in widget has been changed.
	 *
	 * Emited when state of data modifiecation in widget has been changed. For example:
	 * first bit of data is changed, data is saved, data is restored from saved state.
	 */
	void needsSaveChanged(bool newNeedsSave);

};

/**
 * @}
 */

#endif // MODAL_CONFIGURATION_WIDGET_H
