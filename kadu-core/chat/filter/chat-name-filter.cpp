/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "chat/chat.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/buddy-list.h"
#include "contacts/contact-set.h"

#include "chat-name-filter.h"

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Contructs empty ChatNameFilter object that accepts all chats.
 * @param parent parent QObject
 *
 * Contructs empty ChatNameFilter object that accepts all chats.
 */
ChatNameFilter::ChatNameFilter(QObject *parent) :
		ChatFilter(parent)
{
	Filter = new BuddyNameFilter(this);
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Return true if chat name (or its buddies names) matches filter's name.
 * @param chat Chat object that will be checked
 * @return true if chat name (or its buddies names) matches filter's name
 *
 * Return true if chat name (or its buddies names) matches filter's name.
 * For checking buddies name BuddyNameFilter object is used on every
 * buddy in this chat.
 */
bool ChatNameFilter::acceptChat(const Chat &chat)
{
	if (Name.isEmpty())
		return true;

	if (chat.name().contains(Name, Qt::CaseInsensitive))
		return true;

	foreach (const Buddy &buddy, chat.contacts().toBuddySet())
		if (Filter->acceptBuddy(buddy))
			return true;

	return false;
}

/**
 * @author Rafał 'Vogel' Malinowski
 * @short Changes name of this filter.
 *
 * Sets new name to filter chats. If new name is different that old one
 * filterChanged signal is emitted and models can be invalidated.
 */
void ChatNameFilter::setName(const QString& name)
{
	if (name != Name)
	{
		Name = name;
		Filter->setName(Name);
		emit filterChanged();
	}
}
