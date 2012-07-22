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

#ifndef MESSAGE_FILTER_SERVICE_H
#define MESSAGE_FILTER_SERVICE_H

#include <QtCore/QObject>

class Chat;
class MessageFilter;

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class MessageFilterService
 * @short Service for handling filtering of messages.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service is used to handle filtering of messages. It is possible to register MessageFilter instances
 * as outgoing or incoming filters. Calling acceptOutgoingMessage() or acceptIncomingMessage() will call
 * all transformations for given type of message and return true only if all filters accepts given message.
 */
class MessageFilterService : public QObject
{
	QList<MessageFilter *> OutgoingMessageFilters;
	QList<MessageFilter *> IncomingMessageFilters;

public:
	/**
	 * @short Create new instance of MessageFilterService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit MessageFilterService(QObject *parent = 0);
	virtual ~MessageFilterService();

	/**
	 * @short Register instance of MessageFilter as outgoing filter.
	 * @author Rafał 'Vogel' Malinowski
	 * @param filter filter to register
	 *
	 * This method adds given filter to list of filters that will be executed on acceptOutgoingMessage() call.
	 * If filter is already on list then this method will do nothing.
	 */
	void registerOutgoingMessageFilter(MessageFilter *filter);

	/**
	 * @short Unregister instance of MessageFilter as outgoing filter.
	 * @author Rafał 'Vogel' Malinowski
	 * @param filter filter to unregister
	 *
	 * This method removes given filter from list of filters that will be executed on acceptOutgoingMessage() call.
	 * If filter was not on list then this method will do nothing.
	 */
	void unregisterOutgoingMessageFilter(MessageFilter *filter);

	/**
	 * @short Execute all registered outgoing filters on given message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of message
	 * @param message content of message
	 * @return true if all registered outgoing filters accepts given message
	 *
	 * This method executes all outgoing filters on given message and returns true if all registered outgoing
	 * filters accepts given message.
	 */
	bool acceptOutgoingMessage(const Chat &chat, const QString &message);

	/**
	 * @short Register instance of MessageFilter as incoming filter.
	 * @author Rafał 'Vogel' Malinowski
	 * @param filter filter to register
	 *
	 * This method adds given filter to list of filters that will be executed on acceptIncomingMessage() call.
	 * If filter is already on list then this method will do nothing.
	 */
	void registerIncomingMessageFilter(MessageFilter *filter);

	/**
	 * @short Unregister instance of MessageFilter as incoming filter.
	 * @author Rafał 'Vogel' Malinowski
	 * @param filter filter to unregister
	 *
	 * This method removes given filter from list of filters that will be executed on acceptIncomingMessage() call.
	 * If filter was not on list then this method will do nothing.
	 */
	void unregisterIncomingMessageFilter(MessageFilter *filter);

	/**
	 * @short Execute all registered incoming filters on given message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of message
	 * @param message content of message
	 * @return true if all registered incoming filters accepts given message
	 *
	 * This method executes all incoming filters on given message and returns true if all registered incoming
	 * filters accepts given message.
	 */
	bool acceptIncomingMessage(const Chat &chat, const QString &message);

};

/**
 * @}
 */

#endif // MESSAGE_FILTER_SERVICE_H
