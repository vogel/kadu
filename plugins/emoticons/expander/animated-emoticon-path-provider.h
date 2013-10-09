/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
