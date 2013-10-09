/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_CONFIGURATION_H
#define EMOTICON_CONFIGURATION_H

#include "themes/theme.h"

#include "theme/emoticon-theme.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonConfiguration
 * @short Configuration class for emoticons plugin.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class is used to configure emoticons.
 *
 * Properties:
 * <ul>
 *   <li>enabled - enable displaying emoticons</li>
 *   <li>animate - animate displayed emoticons</li>
 *   <li>emoticonTheme - theme of emoticons to display</li>
 * </ul>
 */
class EmoticonConfiguration
{
	bool Enabled;
	bool Animate;
	EmoticonTheme Emoticons;

public:
	EmoticonConfiguration();
	EmoticonConfiguration(const EmoticonConfiguration &copyMe);

	EmoticonConfiguration & operator = (const EmoticonConfiguration &copyMe);

	/**
	 * @short Set value of enabled property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param enabled new value of enabled property
	 */
	void setEnabled(bool enabled);

	/**
	 * @short Return value of enabled property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of enabled property
	 */
	bool enabled() const;

	/**
	 * @short Set value of animate property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param animate new value of animate property
	 */
	void setAnimate(bool animate);

	/**
	 * @short Return value of animate property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of animate property
	 */
	bool animate() const;

	/**
	 * @short Set value of emoticonTheme property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param emoticonTheme new value of emoticonTheme property
	 */
	void setEmoticonTheme(const EmoticonTheme &emoticonTheme);

	/**
	 * @short Return value of emoticonTheme property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of emoticonTheme property
	 */
	EmoticonTheme emoticonTheme() const;

};

/**
 * @}
 */

#endif // EMOTICON_CONFIGURATION_H
