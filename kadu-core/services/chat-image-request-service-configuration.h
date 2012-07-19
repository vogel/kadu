/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_IMAGE_REQUEST_SERVICE_CONFIGURATION_H
#define CHAT_IMAGE_REQUEST_SERVICE_CONFIGURATION_H

#include <QtCore/QtGlobal>

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class ChatImageRequestServiceConfiguration
 * @short Configuration class for ChatImageRequestService.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class is used to configure ChatImageRequestService.
 *
 * Properties:
 * <ul>
 *   <li>limitImageSize - if true then received images have limit on size, if exceeded, then these images will be ignored</li>
 *   <li>maximumImageSizeInKiloBytes - maximum size of image that is accepted without asking (in kilobytes)</li>
 *   <li>allowBiggerImagesAfterAsking - if true then user is asked when image bigger than maximumImageSizeInKiloBytes is about to be receied, if false
 *       then image will be ignored silently</li>
 * </ul>
 */
class ChatImageRequestServiceConfiguration
{
	bool LimitImageSize;
	quint32 MaximumImageSizeInKiloBytes;
	bool AllowBiggerImagesAfterAsking;

public:
	ChatImageRequestServiceConfiguration();
	ChatImageRequestServiceConfiguration(const ChatImageRequestServiceConfiguration &copyMe);

	ChatImageRequestServiceConfiguration & operator = (const ChatImageRequestServiceConfiguration &copyMe);

	/**
	 * @short Set value of limitImageSize property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param limitImageSize new value of limitImageSize property
	 */
	void setLimitImageSize(quint32 limitImageSize);

	/**
	 * @short Return value of limitImageSize property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of limitImageSize property
	 */
	quint32 limitImageSize() const;

	/**
	 * @short Set value of maximumImageSizeInKiloBytes property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param maximumImageSizeInKiloBytes new value of maximumImageSizeInKiloBytes property
	 */
	void setMaximumImageSizeInKiloBytes(quint32 maximumImageSizeInKiloBytes);

	/**
	 * @short Return value of maximumImageSizeInKiloBytes property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of maximumImageSizeInKiloBytes property
	 */
	quint32 maximumImageSizeInKiloBytes() const;

	/**
	 * @short Set value of allowBiggerImagesAfterAsking property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param allowBiggerImagesAfterAsking new value of allowBiggerImagesAfterAsking property
	 */
	void setAllowBiggerImagesAfterAsking(bool allowBiggerImagesAfterAsking);

	/**
	 * @short Return value of allowBiggerImagesAfterAsking property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of allowBiggerImagesAfterAsking property
	 */
	bool allowBiggerImagesAfterAsking() const;

};

/**
 * @}
 */

#endif // CHAT_IMAGE_REQUEST_SERVICE_CONFIGURATION_H
