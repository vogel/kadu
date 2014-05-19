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

class ExecutionArguments;

/**
 * @addtogroup Core
 * @{
 */

/**
 * @class ExecutionArgumentsBuilder
 * @short Builder for creating ExecutionArguments instances from simple values.
 *
 * Example usage:
 * auto builder = ExecutionArgumentsBuilder{};
 * auto arguments = builder.setQueryVersion(true).setOpenIds(QStringList{} << "gg:1" << "gg:2").build();
 *
 * will create ExecutionArguments with QueryVersion set to true and two ids in OpenIds fields. All other
 * fields will have default values.
 */
class KADUAPI ExecutionArgumentsBuilder final
{

public:
	ExecutionArgumentsBuilder();

	ExecutionArgumentsBuilder & setQueryVersion(bool queryVersion);
	ExecutionArgumentsBuilder & setQueryUsage(bool queryUsage);
	ExecutionArgumentsBuilder & setDebugMask(QString debugMask);
	ExecutionArgumentsBuilder & setProfileDirectory(QString profileDirectory);
	ExecutionArgumentsBuilder & setOpenIds(QStringList openIds);

	/**
	 * @short Create ExecutionArguments from values set before by setXXX methods.
	 */
	ExecutionArguments build() const;

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
