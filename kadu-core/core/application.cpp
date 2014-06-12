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
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-storage.h"
#include "configuration/configuration-unusable-exception.h"
#include "configuration/configuration.h"
#include "misc/paths-provider.h"

#include <QtWidgets/QMessageBox>

Application * Application::m_instance = nullptr;

Application * Application::instance()
{
	return m_instance;
}

Application::Application(int &argc, char *argv[]) :
		QApplication{argc, argv}
{
	setApplicationName("Kadu");
	setQuitOnLastWindowClosed(false);

#ifdef Q_OS_WIN32
	// Fix for #2491
	setStyleSheet("QToolBar{border:0px}");
#endif

	m_instance = this;
}

Application::~Application()
{
	m_instance = nullptr;
}

void Application::setPathsProvider(qobject_ptr<PathsProvider> pathsProvider)
{
	m_pathsProvider = std::move(pathsProvider);
}

void Application::readConfiguration() try
{
	auto profilePath = m_pathsProvider->profilePath();

	m_configuration = make_qobject<Configuration>(make_qobject<ConfigurationStorage>(profilePath, this), this);
	m_configuration->read();
}
catch (ConfigurationUnusableException &e)
{
	auto profilePath = e.profilePath();
	auto errorMessage = QCoreApplication::translate("@default", "We're sorry, but Kadu cannot be loaded. "
			"Profile is inaccessible. Please check permissions in the '%1' directory.")
			.arg(profilePath.left(profilePath.length() - 1));
	QMessageBox::critical(0, QCoreApplication::translate("@default", "Profile Inaccessible"), errorMessage, QMessageBox::Abort);

	throw;
}

Configuration * Application::configuration() const
{
	return m_configuration.get();
}

PathsProvider * Application::pathsProvider() const
{
	return m_pathsProvider.get();
}

#include "moc_application.cpp"
