/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QtCore/QDateTime>

#include "contacts/contact.h"

class Chat;
/**
	Structure representing a message.
	\struct Message
	\brief Message
**/
struct Message
{
	Chat *chat;
	Contact sender; /*!< Message sender. */

	QString messageContent; /*!< Message content. */

	QDateTime receiveDate; /*!< Recieve time. */
	QDateTime sendDate; /*!< Send time. */
};

#endif // MESSAGE_H
