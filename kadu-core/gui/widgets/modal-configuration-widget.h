/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QWidget>
#include "exports.h"

#include "configuration-value-state-notifier.h"

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
 * Current state of widget can be determined by calling state method. If
 * data has been changed - widget should not be closed or hidden.
 */
class KADUAPI ModalConfigurationWidget : public QWidget
{
	Q_OBJECT

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
	 * @short Returns configuration state notifier for this widget.
	 * @return configuration state notifier for this widget
	 *
	 * Returns configuration state notifier for this widget.
	 */
	virtual const ConfigurationValueStateNotifier * stateNotifier() const = 0;

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

};

/**
 * @}
 */

#endif // MODAL_CONFIGURATION_WIDGET_H
