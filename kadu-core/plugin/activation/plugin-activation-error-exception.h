/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QString>
#include <exception>

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginActivationErrorException
 * @short Exception thrown in case of plugin activation error.
 *
 * Exception is thrown in case of plugin activation error (like conflicting plugins, unable to find
 * dynamic library file and so on). Exception holds plugin name and human-readable error message.
 */
class PluginActivationErrorException : public std::exception
{

public:
	PluginActivationErrorException(QString pluginName, QString errorMessage) :
			m_pluginName{std::move(pluginName)}, m_errorMessage{std::move(errorMessage)} {}
	virtual ~PluginActivationErrorException() noexcept {}

	QString pluginName() const { return m_pluginName; }
	QString errorMessage() const { return m_errorMessage; }

private:
	QString m_pluginName;
	QString m_errorMessage;

	virtual const char * what() const noexcept { return "Plugin activation error"; }

};

/**
 * @}
 */
