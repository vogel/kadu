/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef MPD_CONFIG
#define MPD_CONFIG

#include "gui/windows/main-configuration-window.h"

class MPDConfig : public ConfigurationAwareObject
{
	QString Host;
	QString Port;
	QString Timeout;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	MPDConfig();
	virtual ~MPDConfig();
	
	const QString & host() const { return Host; }
	const QString & port() const { return Port; }
	const QString & timeout() const { return Timeout; }

};

#endif // MPD_CONFIG
