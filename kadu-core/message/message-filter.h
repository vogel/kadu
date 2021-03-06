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

#ifndef MESSAGE_FILTER_H
#define MESSAGE_FILTER_H

#include "exports.h"

class Message;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class MessageFilter
 * @short Interface to filter message.
 *
 * This interface allows filtering messages.
 */
class KADUAPI MessageFilter
{
public:
    virtual ~MessageFilter()
    {
    }

    /**
     * @short Filter message.
     * @param message message to accept (or not)
     * @return true if message can be accepted, false otherwise
     */
    virtual bool acceptMessage(const Message &message) = 0;
};

/**
 * @}
 */

#endif   // MESSAGE_FILTER_H
