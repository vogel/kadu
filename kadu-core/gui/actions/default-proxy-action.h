/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DEFAULT_PROXY_ACTION_H
#define DEFAULT_PROXY_ACTION_H

#include <QtGui/QAction>

#include "gui/actions/action-description.h"

class NetworkProxy;

class DefaultProxyAction : public ActionDescription
{
	Q_OBJECT

	void populateMenu(QMenu *menu, QActionGroup *actionGroup, NetworkProxy defaultProxy);

private slots:
	void prepareMenu();
	void selectProxyActionTriggered(QAction *action);
	void editProxyConfiguration();

protected:
	virtual QMenu * menuForAction(Action *action);

public:
	explicit DefaultProxyAction(QObject *parent);
	virtual ~DefaultProxyAction();

	virtual QToolButton::ToolButtonPopupMode buttonPopupMode() const
	{
		return QToolButton::InstantPopup;
	}

};

#endif // DEFAULT_PROXY_ACTION_H
