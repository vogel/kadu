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

#ifndef MESSAGE_TRANSFORMER_SERVICE_H
#define MESSAGE_TRANSFORMER_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class Chat;
class MessageTransformer;

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
class KADUAPI MessageTransformerService : public QObject
{
	Q_OBJECT

	QList<MessageTransformer *> OutgoingMessageTransformers;
	QList<MessageTransformer *> IncomingMessageTransformers;

public:
	/**
	 * @short Create new instance of MessageTransformerService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit MessageTransformerService(QObject *parent = 0);
	virtual ~MessageTransformerService();

	/**
	 * @short Register instance of MessageTransformer as outgoing transformer.
	 * @author Rafał 'Vogel' Malinowski
	 * @param transformer transformer to register
	 *
	 * This method adds given transformer to list of transformers that will be executed on transformOutgoingMessage() call.
	 * If transformer is already on list then this method will do nothing.
	 */
	void registerOutgoingMessageTransformer(MessageTransformer *transformer);

	/**
	 * @short Unregister instance of MessageTransformer as outgoing transformer.
	 * @author Rafał 'Vogel' Malinowski
	 * @param transformer transformer to unregister
	 *
	 * This method removes given transformer from list of transformers that will be executed on transformOutgoingMessage() call.
	 * If transformer was not on list then this method will do nothing.
	 */
	void unregisterOutgoingMessageTransformer(MessageTransformer *transformer);

	/**
	 * @short Execute all registered outgoing transformers on given message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of message
	 * @param message content of message
	 * @return transformed message
	 *
	 * This method executes all outgoing transformers on given message and returns combined result of these transformations.
	 */
	QString transformOutgoingMessage(const Chat &chat, const QString &message);

	/**
	 * @short Register instance of MessageTransformer as incoming transformer.
	 * @author Rafał 'Vogel' Malinowski
	 * @param transformer transformer to register
	 *
	 * This method adds given transformer to list of transformers that will be executed on transformIncomingMessage() call.
	 * If transformer is already on list then this method will do nothing.
	 */
	void registerIncomingMessageTransformer(MessageTransformer *transformer);

	/**
	 * @short Unregister instance of MessageTransformer as incoming transformer.
	 * @author Rafał 'Vogel' Malinowski
	 * @param transformer transformer to unregister
	 *
	 * This method removes given transformer from list of transformers that will be executed on transformIncomingMessage() call.
	 * If transformer was not on list then this method will do nothing.
	 */
	void unregisterIncomingMessageTransformer(MessageTransformer *transformer);

	/**
	 * @short Execute all registered incoming transformers on given message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of message
	 * @param message content of message
	 * @return transformed message
	 *
	 * This method executes all incoming transformers on given message and returns combined result of these transformations.
	 */
	QString transformIncomingMessage(const Chat &chat, const QString &message);

};

/**
 * @}
 */

#endif // MESSAGE_TRANSFORMER_SERVICE_H
