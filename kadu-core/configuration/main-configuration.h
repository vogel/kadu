/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MAIN_CONFIGURATION_H
#define MAIN_CONFIGURATION_H

#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"

class MainConfiguration : public QObject, private ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MainConfiguration)

	static MainConfiguration * Instance;

	bool SimpleMode;

	MainConfiguration();
	virtual ~MainConfiguration();

protected:
	virtual void configurationUpdated();

public:
	static MainConfiguration * instance();

	static void createInstance();
	static void destroyInstance();

	bool simpleMode() { return SimpleMode; }

signals:
	void simpleModeChanged();

};

#endif // MAIN_CONFIGURATION_H
