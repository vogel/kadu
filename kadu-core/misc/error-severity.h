/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ERROR_SEVERITY_H
#define ERROR_SEVERITY_H

/**
 * @addtogroup Misc
 * @{
 */

/**
 * @enum ErrorSeverity
 * @short Error severity enum.
 * @author Rafał 'Vogel' Malinowski
 */
enum ErrorSeverity
{
	/**
	 * @short No error - everything is fine.
	 * @author Rafał 'Vogel' Malinowski
	 */
	NoError,

	/**
	 * @short Low severity error - can be ignored.
	 * @author Rafał 'Vogel' Malinowski
	 */
	ErrorLow,

	/**
	 * @short High severity error - should not be ignored.
	 * @author Rafał 'Vogel' Malinowski
	 */
	ErrorHigh
};

/**
 * @}
 */

#endif // ERROR_SEVERITY_H
