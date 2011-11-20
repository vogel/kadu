/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "plugins/generic-plugin.h"

/*!
 * This class provides autohiding Kadu's main window after preset time.
 * \brief This class provides autohiding Kadu's main window after preset time
 */
class AutoHide : public ConfigurationUiHandler, ConfigurationAwareObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

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

	virtual int init(bool firstLoad);
	virtual void done();

};

#endif // AUTO_HIDE_H
