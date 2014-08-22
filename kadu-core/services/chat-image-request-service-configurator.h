/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CHAT_IMAGE_REQUEST_SERVICE_CONFIGURATOR_H
#define CHAT_IMAGE_REQUEST_SERVICE_CONFIGURATOR_H

#include <QtCore/QPointer>

#include "configuration/configuration-aware-object.h"

class ChatImageRequestService;

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class ChatImageRequestServiceConfigurator
 * @short Standard configuration setter for ChatImageRequestService.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class listens to changed in global configuration. After change is detected it updated configuration of given
 * ChatImageRequestService with newly created ChatImageRequestServiceConfiguration instance.
 */
class ChatImageRequestServiceConfigurator : private ConfigurationAwareObject
{
	QPointer<ChatImageRequestService> Service;

protected:
	virtual void configurationUpdated();

public:
	/**
	 * @short Sets ChatImageRequestService to configure.
	 * @author Rafał 'Vogel' Malinowski
	 * @param service service to configured
	 */
	void setChatImageRequestService(ChatImageRequestService *service);

};

/**
 * @}
 */

#endif // CHAT_IMAGE_REQUEST_SERVICE_CONFIGURATOR_H
