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

#ifndef ANIMATED_EMOTICON_PATH_PROVIDER_H
#define ANIMATED_EMOTICON_PATH_PROVIDER_H

#include "expander/emoticon-path-provider.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class AnimatedEmoticonPathProvider
 * @short EmoticonPathProvider that returns path to animated version of emoticon.
 * @author Rafał 'Vogel' Malinowski
 */
class AnimatedEmoticonPathProvider : public EmoticonPathProvider
{
public:
	virtual ~AnimatedEmoticonPathProvider();

	/**
	 * @short Return path to animated version of emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @param emoticon emoticon to get path for
	 * @return path to animated file of emoticon
	 */
	virtual QString emoticonPath(const Emoticon &emoticon);

};

/**
 * @}
 */

#endif // ANIMATED_EMOTICON_PATH_PROVIDER_H
