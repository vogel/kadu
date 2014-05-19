/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "execution-arguments-builder.h"

#include "execution-arguments/execution-arguments.h"

ExecutionArgumentsBuilder::ExecutionArgumentsBuilder() :
		m_queryVersion{false},
		m_queryUsage{false}
{
}

ExecutionArgumentsBuilder & ExecutionArgumentsBuilder::setQueryVersion(bool queryVersion)
{
	m_queryVersion = queryVersion;
	return *this;
}

ExecutionArgumentsBuilder & ExecutionArgumentsBuilder::setQueryUsage(bool queryUsage)
{
	m_queryUsage = queryUsage;
	return *this;
}

ExecutionArgumentsBuilder & ExecutionArgumentsBuilder::setDebugMask(QString debugMask)
{
	m_debugMask = std::move(debugMask);
	return *this;
}

ExecutionArgumentsBuilder & ExecutionArgumentsBuilder::setProfileDirectory(QString profileDirectory)
{
	m_profileDirectory = std::move(profileDirectory);
	return *this;
}

ExecutionArgumentsBuilder & ExecutionArgumentsBuilder::setOpenIds(QStringList openIds)
{
	m_openIds = std::move(openIds);
	return *this;
}

ExecutionArguments ExecutionArgumentsBuilder::build() const
{
	return {
		m_queryVersion,
		m_queryUsage,
		m_debugMask,
		m_profileDirectory,
		m_openIds
	};
}
