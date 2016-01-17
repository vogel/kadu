/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "configuration/configuration-aware-object.h"

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>

class Configuration;
class Idle;

class AutoHide : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AutoHide(QObject *parent = nullptr);
	virtual ~AutoHide();

private:
	QPointer<Configuration> m_configuration;
	QPointer<Idle> m_idle;

	QTimer m_timer;
	int m_idleTime;
	bool m_enabled;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIdle(Idle *idle);
	INJEQT_INIT void init();

	void timerTimeoutSlot();

protected:
	virtual void configurationUpdated();

};
