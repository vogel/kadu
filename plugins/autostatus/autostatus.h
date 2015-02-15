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

#ifndef AUTOSTATUS_H
#define AUTOSTATUS_H

#include <QtCore/QStringList>

#include "configuration/autostatus-configuration.h"

class QTimer;

class AutostatusStatusChanger;

/*!
 * This class provides functionality for automatic status change
 * after served time
 * \brief Automatic status change class
 */
class Autostatus : public QObject
{
	Q_OBJECT

	static Autostatus *Instance;

	AutostatusConfiguration Configuration;

	QTimer *Timer;
	AutostatusStatusChanger *MyStatusChanger;

	Autostatus();
	virtual ~Autostatus();

	void on();
	void off();

	bool readDescriptionList();

	QStringList::ConstIterator CurrentDescription;
	QStringList DescriptionList;

private slots:
	//! This slot is called on timeout
	void changeStatus();

public:
	static void createInstance();
	static void destroyInstance();

	static Autostatus * instance() { return Instance; }

	void toggle(bool toggled);

};

#endif // AUTOSTATUS_H
