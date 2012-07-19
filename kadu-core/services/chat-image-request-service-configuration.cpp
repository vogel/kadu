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

#include "chat-image-request-service-configuration.h"

ChatImageRequestServiceConfiguration::ChatImageRequestServiceConfiguration() :
		LimitImageSize(false), MaximumImageSizeInKiloBytes(255 * 1024), AllowBiggerImagesAfterAsking(true)
{
}

ChatImageRequestServiceConfiguration::ChatImageRequestServiceConfiguration(const ChatImageRequestServiceConfiguration &copyMe)
{
	LimitImageSize = copyMe.LimitImageSize;
	MaximumImageSizeInKiloBytes = copyMe.MaximumImageSizeInKiloBytes;
	AllowBiggerImagesAfterAsking = copyMe.AllowBiggerImagesAfterAsking;
}

ChatImageRequestServiceConfiguration & ChatImageRequestServiceConfiguration::operator = (const ChatImageRequestServiceConfiguration &copyMe)
{
	LimitImageSize = copyMe.LimitImageSize;
	MaximumImageSizeInKiloBytes = copyMe.MaximumImageSizeInKiloBytes;
	AllowBiggerImagesAfterAsking = copyMe.AllowBiggerImagesAfterAsking;

	return *this;
}

void ChatImageRequestServiceConfiguration::setLimitImageSize (quint32 limitImageSize)
{
	LimitImageSize = limitImageSize;
}

quint32 ChatImageRequestServiceConfiguration::limitImageSize() const
{
	return LimitImageSize;
}

void ChatImageRequestServiceConfiguration::setMaximumImageSizeInKiloBytes(quint32 maximumImageSizeInKiloBytes)
{
	MaximumImageSizeInKiloBytes = maximumImageSizeInKiloBytes;
}

quint32 ChatImageRequestServiceConfiguration::maximumImageSizeInKiloBytes() const
{
	return MaximumImageSizeInKiloBytes;
}

void ChatImageRequestServiceConfiguration::setAllowBiggerImagesAfterAsking(bool allowBiggerImagesAfterAsking)
{
	AllowBiggerImagesAfterAsking = allowBiggerImagesAfterAsking;
}

bool ChatImageRequestServiceConfiguration::allowBiggerImagesAfterAsking() const
{
	return AllowBiggerImagesAfterAsking;
}
