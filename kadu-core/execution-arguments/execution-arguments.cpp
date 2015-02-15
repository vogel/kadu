/*
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

#include "execution-arguments.h"

ExecutionArguments::ExecutionArguments(bool queryVersion, bool queryUsage, QString debugMask, QString profileDirectory, QStringList openIds) :
		m_queryVersion{queryVersion},
		m_queryUsage{queryUsage},
		m_debugMask{std::move(debugMask)},
		m_profileDirectory{std::move(profileDirectory)},
		m_openIds{std::move(openIds)}
{
}

bool ExecutionArguments::queryVersion() const
{
	return m_queryVersion;
}

bool ExecutionArguments::queryUsage() const
{
	return m_queryUsage;
}

QString ExecutionArguments::debugMask() const
{
	return m_debugMask;
}

QString ExecutionArguments::profileDirectory() const
{
	return m_profileDirectory;
}

QStringList ExecutionArguments::openIds() const
{
	return m_openIds;
}
