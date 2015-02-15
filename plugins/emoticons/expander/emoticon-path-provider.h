/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_PATH_PROVIDER_H
#define EMOTICON_PATH_PROVIDER_H

#include <QtCore/QString>

#include "emoticon.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonPathProvider
 * @short Interface to get path of emoticon file from Emoticon instance.
 * @author Rafał 'Vogel' Malinowski
 */
class EmoticonPathProvider
{
public:
	virtual ~EmoticonPathProvider() {}

	/**
	 * @short Return path of emoticon file for given emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @param emoticon emoticon to get path for
	 * @return path to file of emoticon
	 */
	virtual QString emoticonPath(const Emoticon &emoticon) = 0;

};

/**
 * @}
 */

#endif // EMOTICON_PATH_PROVIDER_H
