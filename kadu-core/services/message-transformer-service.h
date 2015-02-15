/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef MESSAGE_TRANSFORMER_SERVICE_H
#define MESSAGE_TRANSFORMER_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

#include "message/message-transformer.h"
#include "services/transformer-service.h"

class Chat;

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class MessageTransformerService
 * @short Service for handling transformations of messages.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service is used to handle transformations of messages. It is possible to register MessageTransformer instances
 * as outgoing or incoming transformers. Calling transformOutgoingMessage() or transformIncomingMessage() will call
 * all transformations for given type of message and return combined result of all transformations.
 */
class KADUAPI MessageTransformerService : public QObject, public TransformerService<MessageTransformer>
{
	Q_OBJECT

public:
	/**
	 * @short Create new instance of MessageTransformerService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit MessageTransformerService(QObject *parent = 0);
	virtual ~MessageTransformerService();

};

/**
 * @}
 */

#endif // MESSAGE_TRANSFORMER_SERVICE_H
