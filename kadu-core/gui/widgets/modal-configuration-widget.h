/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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
#include "exports.h"

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @enum ModalConfigurationWidgetState
 * @author Rafal 'Vogel' Malinowski
 * @short State of modal configuration widget.
 *
 * Modal configuration window can be in one of three states:
 * <ul>
 *   <li>no data in widget has been changed</li>
 *   <li>data in widget has been changed and is valid (can be stored)</li>
 *   <li>data in widget has been changed and is invalid (can not be stored)</li>
 * </ul>
 */
enum ModalConfigurationWidgetState
{
	/**
	 * No data in widget has been changed.
	 */
	StateNotChanged,
	/**
	 * Data in widget has been changed and is valid (can be stored).
	 */
	StateChangedDataValid,
	/**
	 * Data in widget has been changed and is invalid (can not be stored).
	 */
	StateChangedDataInvalid
};

/**
 * @class ModalConfigurationWidget
 * @author Rafal 'Vogel' Malinowski
 * @short Widget that should not be hidden when it contains modified data.
 *
 * This widget allows to edit some data. When data is modified and not saved
 * parent widget should not allow to hide it - it should either force saving
 * data or rejecting it (by apply and cancel methods).
 *
 * Current state of widget can be determined by calling state method. If
 * data has been changed - widget should not be closed or hidden.
 */
class KADUAPI ModalConfigurationWidget : public QWidget
{
	Q_OBJECT

	ModalConfigurationWidgetState State;

protected:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Updates state of widget.
	 *
	 * Sets new state value. When new value is different from
	 * old one, stateChanged signal is emitted.
	 */
	void setState(ModalConfigurationWidgetState state);

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
	 * @short Returns state of widget.
	 * @return state of widget
	 *
	 * Returns state of widget.
	 */
	ModalConfigurationWidgetState state();

public slots:
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

signals:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted when state of data modifiecation in widget has been changed.
	 *
	 * Emitted when state of data modifiecation in widget has been changed. For example:
	 * first bit of data is changed, data is saved, data is restored from saved state.
	 */
	void stateChanged(ModalConfigurationWidgetState state);

};

/**
 * @}
 */

#endif // MODAL_CONFIGURATION_WIDGET_H
