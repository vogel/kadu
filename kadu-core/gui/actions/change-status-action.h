/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHANGE_STATUS_ACTION_H
#define CHANGE_STATUS_ACTION_H

#include <QtWidgets/QAction>

#include "gui/actions/action-description.h"

class ChangeStatusAction : public ActionDescription
{
	Q_OBJECT

protected:
	virtual QMenu * menuForAction(Action *action);
	virtual void actionInstanceCreated(Action *action);

public:
	explicit ChangeStatusAction(QObject *parent);
	virtual ~ChangeStatusAction();

	virtual QToolButton::ToolButtonPopupMode buttonPopupMode() const
	{
		return QToolButton::InstantPopup;
	}

};

#endif // CHANGE_STATUS_ACTION_H
