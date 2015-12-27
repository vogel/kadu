/*
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <injeqt/injeqt.h>

#include "configuration/autostatus-configuration.h"

class AutostatusStatusChanger;

class QTimer;

/*!
 * This class provides functionality for automatic status change
 * after served time
 * \brief Automatic status change class
 */
class AutostatusService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AutostatusService(QObject *parent = nullptr);
	virtual ~AutostatusService();

	void toggle(bool toggled);

private:
	QPointer<AutostatusStatusChanger> m_autostatusStatusChanger;

	AutostatusConfiguration Configuration;

	QTimer *Timer;

	void on();
	void off();

	bool readDescriptionList();

	QStringList::ConstIterator CurrentDescription;
	QStringList DescriptionList;

private slots:
	INJEQT_SETTER void setAutostatusStatusChanger(AutostatusStatusChanger *autostatusStatusChanger);

	//! This slot is called on timeout
	void changeStatus();

};
