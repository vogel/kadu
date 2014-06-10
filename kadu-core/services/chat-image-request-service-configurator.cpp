/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "services/chat-image-request-service-configuration.h"
#include "services/chat-image-request-service.h"

#include "chat-image-request-service-configurator.h"

void ChatImageRequestServiceConfigurator::setChatImageRequestService(ChatImageRequestService *service)
{
	Service = service;

	configurationUpdated();
}

void ChatImageRequestServiceConfigurator::configurationUpdated()
{
	if (!Service)
		return;

	ChatImageRequestServiceConfiguration configuration;

	configuration.setLimitImageSize(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "LimitImageSize", true));
	configuration.setMaximumImageSizeInKiloBytes(Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("Chat", "MaximumImageSizeInKiloBytes", 255));
	configuration.setAllowBiggerImagesAfterAsking(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "AllowBiggerImagesAfterAsking", true));

	Service.data()->setConfiguration(configuration);
}
