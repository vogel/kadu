/*
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2005, 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2007, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QObject>

#ifndef Q_MOC_RUN
#  define INJEQT_SETTER
#endif

class Configuration;
class ConfigurationWriter;
class PathsProvider;

class KADUAPI Application : public QObject
{
	Q_OBJECT

public:
	static Application * instance();

	Q_INVOKABLE Application();
	virtual ~Application();

	Configuration * configuration() const;
	PathsProvider * pathsProvider() const;

	void flushConfiguration();
	void backupConfiguration();

	bool isSavingSession() const;
	void quit();

private:
	static Application * m_instance;

	Configuration *m_configuration;
	ConfigurationWriter *m_configurationWriter;
	PathsProvider *m_pathsProvider;

public slots:
	INJEQT_SETTER void setConfiguration(Configuration *configuration);
	INJEQT_SETTER void setConfigurationWriter(ConfigurationWriter *configurationWriter);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
