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

#pragma once

#include "exports.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

/**
 * @addtogroup Core
 * @{
 */

/**
 * @class ExecutionArguments
 * @short Arguments used to run Kadu.
 *
 * All run arguments recognized by Kadu are stored in this class.
 */
class KADUAPI ExecutionArguments final
{

public:
	ExecutionArguments(bool queryVersion, bool queryUsage, QString debugMask, QString profileDirectory, QStringList openIds);

	/**
	 * @return True if version information is requested.
	 */
	bool queryVersion() const;

	/**
	 * @return True if usage information is requested.
	 */
	bool queryUsage() const;

	/**
	 * @return Requested value of debug mask.
	 * @todo change to int
	 */
	QString debugMask() const;

	/**
	 * @return Requested value of profile directory.
	 */
	QString profileDirectory() const;

	/**
	 * @return Requested list of ids to open chat with.
	 */
	QStringList openIds() const;

private:
	bool m_queryVersion;
	bool m_queryUsage;
	QString m_debugMask;
	QString m_profileDirectory;
	QStringList m_openIds;

};

/**
 * @}
 */
