/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_BUTTONS_H
#define STATUS_BUTTONS_H

#include <QtCore/QMap>
#include <QtWidgets/QToolBar>

#include "status/status-container-aware-object.h"

class StatusButton;
class StatusContainer;

class KADUAPI StatusButtons : public QToolBar, private StatusContainerAwareObject
{
	Q_OBJECT

	QMap<StatusContainer *, StatusButton *> Buttons;

	void enableStatusName();
	void disableStatusName();

protected:
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	explicit StatusButtons(QWidget *parent = 0);
	virtual ~StatusButtons();

};

#endif // STATUS_BUTTONS_H
