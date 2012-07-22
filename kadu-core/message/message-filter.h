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

#ifndef MESSAGE_FILTER_H
#define MESSAGE_FILTER_H

#include <QtCore/QObject>

class Chat;
class Contact;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class MessageFilter
 * @short Interface to filter message.
 * @author Rafał 'Vogel' Malinowski
 *
 * This interface allows filtering messages.
 */
class MessageFilter : public QObject
{
	Q_OBJECT

protected:
	explicit MessageFilter(QObject *parent = 0);
	virtual ~MessageFilter();

public:
	/**
	 * @short Filter message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of message
	 * @param sender sender of message
	 * @param message content of message
	 * @return true if message can be accepted, false otherwise
	 */
	virtual bool acceptMessage(const Chat &chat, const Contact &sender, const QString &message) = 0;

};

/**
 * @}
 */

#endif // MESSAGE_FILTER_H
