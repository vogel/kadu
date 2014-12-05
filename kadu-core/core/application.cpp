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
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/application.h"

#include "configuration/configuration-writer.h"
#include "configuration/configuration.h"
#include "misc/paths-provider.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

Application * Application::m_instance = nullptr;

Application * Application::instance()
{
	return m_instance;
}

Application::Application() :
		m_configuration{nullptr},
		m_configurationWriter{nullptr},
		m_pathsProvider{nullptr}
{
#ifdef Q_OS_WIN32
	// Fix for #2491
	qApp->setStyleSheet("QToolBar{border:0px}");
#endif

	m_instance = this;
}

Application::~Application()
{
	m_instance = nullptr;
}

void Application::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void Application::setConfigurationWriter(ConfigurationWriter *configurationWriter)
{
	m_configurationWriter = configurationWriter;
}

void Application::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

Configuration * Application::configuration() const
{
	return m_configuration;
}

PathsProvider * Application::pathsProvider() const
{
	return m_pathsProvider;
}

void Application::flushConfiguration()
{
	m_configurationWriter->write();
}

void Application::backupConfiguration()
{
	m_configurationWriter->backup();
}

bool Application::isSavingSession() const
{
	return qApp->isSavingSession();
}

void Application::quit()
{
	qApp->quit();
}

#include "moc_application.cpp"
