/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef WAIT_OVERLAY_H
#define WAIT_OVERLAY_H

#include <QtGui/QLabel>

#include "exports.h"

class QLabel;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class WaitOverlay
 * @author Rafał 'Vogel' Malinowski
 * @short Widget that shows wait animation over any other widget.
 *
 * This widget is a gray overlay with wait animation over any other widget.
 * To show overlay just create new WaitOverlay with target widget as parent.
 * To remove overlay delete this new widget.
 *
 * This widget is hidden by default. It shows itself after 0.5 seconds of existence
 * so it does not show for very short operations.
 */
class KADUAPI WaitOverlay : public QLabel
{
	Q_OBJECT

private slots:
	void timeoutPassed();

protected:
	virtual bool eventFilter(QObject *object, QEvent *event);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new WaitOverlay widget.
	 * @param parent parent widget
	 *
	 * This contructor creates WaitOverlay widget and attaches to given parent.
	 * Overlay will follow target widget and resize with it.
	 */
	explicit WaitOverlay(QWidget *parent = 0);
	virtual ~WaitOverlay();

};

/**
 * @}
 */

#endif // WAIT_OVERLAY_H
