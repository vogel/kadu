/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef ABSTRACT_CHAT_FILTER
#define ABSTRACT_CHAT_FILTER

#include <QtCore/QObject>

#include "exports.h"

class Chat;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Abstract class for filtering chats.
 *
 * Abstract class for filtering chats. Derivered classes can be used in models
 * to filter out some chats. Override acceptChat method to return false if
 * chat should be filtered out. Emit filterChanged signal to inform model
 * that setting of this filter has changed and model needs to be invalidated.
 */
class KADUAPI ChatFilter : public QObject
{
	Q_OBJECT

public:

	/**
	* @author Rafał 'Vogel' Malinowski
	* @short Contructs empty ChatFilter object.
	* @param parent parent QObject
	*
	* Contructs empty ChatFilter object.
	*/
	ChatFilter(QObject *parent = 0) : QObject(parent) {}

	/**
	* @author Rafał 'Vogel' Malinowski
	* @short Returns false if chat should be filtered out.
	* @param chat Chat object to be tested
	*
	* Returns false if chat should be filtered out.
	*/
	virtual bool acceptChat(const Chat &chat) = 0;

signals:
	/**
	* @author Rafał 'Vogel' Malinowski
	* @short Emitted when filter settings has changed.
	*
	* Signal should be emitted every time filter settings has changed
	* and model should be invalidated to filter items using these
	* new settings.
	*/
	void filterChanged();

};

/**
 * @}
 */

#endif // ABSTRACT_CHAT_FILTER
