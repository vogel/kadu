/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef AUTO_HIDE_H
#define AUTO_HIDE_H

#include <QtCore/QTimer>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

/*!
 * This class provides autohiding Kadu's main window after preset time.
 * \brief This class provides autohiding Kadu's main window after preset time
 */
class AutoHide : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	QTimer Timer;
	int IdleTime;
	bool Enabled;

private slots:
	void timerTimeoutSlot();

protected:
	virtual void configurationUpdated();
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public:
	explicit AutoHide(QObject *parent = 0);

	virtual ~AutoHide();
};

extern AutoHide *autoHide;

#endif // AUTO_HIDE_H
