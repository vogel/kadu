/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STANDARD_URL_EXPANDER_CONFIGURATION_H
#define STANDARD_URL_EXPANDER_CONFIGURATION_H

#include <QtCore/QString>

/**
 * @addtogroup Autoresponder
 * @{
 */

/**
 * @class StandardUrlExpanderConfiguration
 * @short Configuration class for StandardUrlExpanderConfiguration.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class is used to configure StandardUrlExpanderConfiguration.
 *
 * Properties:
 * <ul>
 *   <li>foldLink - if links should be folded</li>
 *   <li>foldLinkThreshold - maximum length of link before folding</li>
 * </ul>
 */
class StandardUrlExpanderConfiguration
{
	bool FoldLink;
	int FoldLinkThreshold;

public:
	StandardUrlExpanderConfiguration();
	StandardUrlExpanderConfiguration(const StandardUrlExpanderConfiguration &copyMe);

	StandardUrlExpanderConfiguration & operator = (const StandardUrlExpanderConfiguration &copyMe);

	/**
	 * @short Set value of foldLink property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param autoRespondText new value of foldLink property
	 */
	void setFoldLink(bool foldLink);

	/**
	 * @short Return value of foldLink property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of foldLink property
	 */
	bool foldLink() const;

	/**
	 * @short Set value of foldLinkThreshold property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param foldLinkThreshold new value of foldLinkThreshold property
	 */
	void setFoldLinkThreshold(int foldLinkThreshold);

	/**
	 * @short Return value of foldLinkThreshold property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of foldLinkThreshold property
	 */
	int foldLinkThreshold() const;

};

/**
 * @}
 */

#endif // STANDARD_URL_EXPANDER_CONFIGURATION_H
