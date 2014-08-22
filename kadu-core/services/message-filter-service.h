/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MESSAGE_FILTER_SERVICE_H
#define MESSAGE_FILTER_SERVICE_H

#include <QtCore/QObject>

#include "exports.h"

class Message;
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
 * This service is used to handle filtering of messages. Calling acceptMessage() to check all installed
 * filters - true will be returned only if all filters accepts given message.
 */
class KADUAPI MessageFilterService : public QObject
{
	Q_OBJECT

	QList<MessageFilter *> MessageFilters;

public:
	/**
	 * @short Create new instance of MessageFilterService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit MessageFilterService(QObject *parent = 0);
	virtual ~MessageFilterService();

	/**
	 * @short Register new filter.
	 * @author Rafał 'Vogel' Malinowski
	 * @param filter filter to register
	 *
	 * This method adds given filter to list of filters that will be executed on acceptMessage() call.
	 * If filter is already on list then this method will do nothing.
	 */
	void registerMessageFilter(MessageFilter *filter);

	/**
	 * @short Unregister filter.
	 * @author Rafał 'Vogel' Malinowski
	 * @param filter filter to unregister
	 *
	 * This method removes given filter from list of filters that will be executed on acceptMessage() call.
	 * If filter was not on list then this method will do nothing.
	 */
	void unregisterMessageFilter(MessageFilter *filter);

	/**
	 * @short Execute all registered filters on given message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param message message to filter
	 * @return true if all registered filters accepts given message
	 *
	 * This method executes all filters on given message and returns true if all of them accepts given message.
	 */
	bool acceptMessage(const Message &message);

};

/**
 * @}
 */

#endif // MESSAGE_FILTER_SERVICE_H
