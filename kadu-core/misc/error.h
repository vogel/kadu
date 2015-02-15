/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ERROR_H
#define ERROR_H

#include <QtCore/QString>

#include "misc/error-severity.h"
#include "exports.h"

/**
 * @addtogroup Misc
 * @{
 */

/**
 * @class Error
 * @short Simple class describing an error.
 * @author Rafał 'Vogel' Malinowski
 *
 * Erro contains severity info and error message string.
 */
class KADUAPI Error
{
	ErrorSeverity Severity;
	QString Message;

public:
	/**
	 * @short Create new Error class.
	 * @author Rafał 'Vogel' Malinowski
	 * @param severity this error severity
	 * @param message this error message
	 */
	Error(ErrorSeverity severity, const QString &message);

	/**
	 * @short Copy Error object.
	 * @author Rafał 'Vogel' Malinowski
	 * @param copyMe Error object to copy
	 */
	Error(const Error &copyMe);

	/**
	 * @short Copy Error object and return this object.
	 * @author Rafał 'Vogel' Malinowski
	 * @param copyMe Error object to copy
	 * @return this object
	 */
	Error & operator = (const Error &copyMe);

	/**
	 * @short Return severity of this object.
	 * @author Rafał 'Vogel' Malinowski
	 * @return severity of this object
	 */
	ErrorSeverity severity() const;

	/**
	 * @short Return message of this object.
	 * @author Rafał 'Vogel' Malinowski
	 * @return message of this object
	 */
	QString message() const;

};

/**
 * @}
 */

#endif // ERROR_H
