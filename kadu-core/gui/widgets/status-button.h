/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_BUTTON_H
#define STATUS_BUTTON_H

#include <QtGui/QPushButton>

#include "configuration/configuration-aware-object.h"
#include "status/status.h"

class StatusContainer;
class StatusMenu;

class StatusButton : public QPushButton, private ConfigurationAwareObject
{
	Q_OBJECT

	StatusContainer *MyStatusContainer;
	bool DisplayStatusName;

	void createGui();
	void update();

private slots:
	void statusChanged();

protected:
	virtual void configurationUpdated();

public:
	explicit StatusButton(StatusContainer *statusContainer, QWidget *parent = 0);
	virtual ~StatusButton();

	void setDisplayStatusName(bool displayStatusName);

};

#endif // STATUS_BUTTON_H
