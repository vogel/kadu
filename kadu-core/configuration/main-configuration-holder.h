/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MAIN_CONFIGURATION_HOLDER_H
#define MAIN_CONFIGURATION_HOLDER_H

#include "configuration-holder.h"

enum SetStatusMode
{
	SetStatusPerAccount,
	SetStatusPerIdentity,
	SetStatusForAll
};

class MainConfigurationHolder : public ConfigurationHolder
{
	Q_OBJECT
	Q_DISABLE_COPY(MainConfigurationHolder)

	static MainConfigurationHolder *Instance;
	explicit MainConfigurationHolder();

	SetStatusMode SetStatus;

public:
	static void createInstance();
	static void destroyInstance();
	static MainConfigurationHolder * instance();
	void configurationUpdated();

	SetStatusMode setStatusMode() const { return SetStatus; }
	bool isSetStatusPerAccount() const { return SetStatus == SetStatusPerAccount; }
	bool isSetStatusPerIdentity() const { return SetStatus == SetStatusPerIdentity; }

signals:
	void setStatusModeChanged();

};

#endif // MAIN_CONFIGURATION_HOLDER_H
