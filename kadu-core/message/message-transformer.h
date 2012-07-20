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

#ifndef MESSAGE_TRANSFORMER_H
#define MESSAGE_TRANSFORMER_H

#include <QtCore/QObject>

class Chat;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class MessageTransformer
 * @short Interface to transform content of message.
 * @author Rafał 'Vogel' Malinowski
 *
 * This interface allows arbitraty tranformations of message content. It can be used to encypt/decrypt messages, cenzor them
 * or change language to 1337.
 */
class MessageTransformer : public QObject
{
	Q_OBJECT

protected:
	explicit MessageTransformer(QObject *parent = 0);
	virtual ~MessageTransformer();

public:
	/**
	 * @short Transform content of message.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chat chat of message
	 * @param message content of message
	 * @return tranformed message
	 */
	virtual QString transformMessage(const Chat &chat, const QString &message) = 0;

};

/**
 * @}
 */

#endif // MESSAGE_TRANSFORMER_H
