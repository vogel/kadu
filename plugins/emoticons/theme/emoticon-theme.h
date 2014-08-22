/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_THEME_H
#define EMOTICON_THEME_H

#include <QtCore/QVector>

#include "emoticon.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonTheme
 * @short Representation of emoticon theme as list of emoticons.
 * @author Rafał 'Vogel' Malinowski
 *
 * Emoticon theme is represented by two list. First one, called emoticons(), contains each emoticon image only once
 * with the best trigger text existing. Second one, called aliases(), contains all versions of emoticons (including
 * these from emoticons()) with all versions of trigger text.
 */
class EmoticonTheme
{
	QVector<Emoticon> Emoticons;
	QVector<Emoticon> Aliases;

public:
	/**
	 * @short Create emoticon theme with no emoticons.
	 * @author Rafał 'Vogel' Malinowski
	 */
	EmoticonTheme();

	/**
	 * @short Create emoticon theme with given emoticons and aliases.
	 * @author Rafał 'Vogel' Malinowski
	 * @param emoticons emoticons of new theme
	 * @param aliases aliases of emoticons in new theme
	 *
	 * Please note that aliases should be a superset of emoticons.
	 */
	EmoticonTheme(const QVector<Emoticon> &emoticons, const QVector<Emoticon> &aliases);

	/**
	 * @short Creates a copy of emoticon theme.
	 * @author Rafał 'Vogel' Malinowski
	 * @param copyMe emoticon theme to copy
	 */
	EmoticonTheme(const EmoticonTheme &copyMe);

	EmoticonTheme & operator = (const EmoticonTheme &copyMe);

	/**
	 * @short Return collection of emoticons.
	 * @author Rafał 'Vogel' Malinowski
	 * @return collection of emoticons
	 *
	 * Returned collection contains each emoticon image only once with best trigger text.
	 */
	QVector<Emoticon> emoticons() const;

	/**
	 * @short Return collection of aliases.
	 * @author Rafał 'Vogel' Malinowski
	 * @return collection of aliases
	 *
	 * Returned collection contains each emoticon image with each trigger text for that image.
	 */
	QVector<Emoticon> aliases() const;

};

/**
 * @}
 */

#endif // EMOTICON_THEME_H
