/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "chat/chat-details-contact.h"
#include "chat/chat-manager.h"
#include "contacts/contact-set.h"
#include "icons/kadu-icon.h"

#include "chat-type-contact.h"

Chat ChatTypeContact::findChat(const Contact &contact, NotFoundAction notFoundAction)
{
	Account account = contact.contactAccount();
	if (!account)
		return Chat::null;

	if (contact.id() == account.id())
		return Chat::null;

	foreach (const Chat &chat, ChatManager::instance()->allItems())
		if (chat.type() == QLatin1String("Contact") || chat.type() == QLatin1String("Simple"))
			if (chat.contacts().toContact() == contact)
			{
				// when contacts changed their accounts we need to change account of chat too
				chat.setChatAccount(account);
				return chat;
			}

	if (ActionReturnNull == notFoundAction)
		return Chat::null;

	Chat chat = Chat::create();
	chat.setChatAccount(account);
	chat.setType("Contact");

	ChatDetailsContact *chatDetailsContact = dynamic_cast<ChatDetailsContact *>(chat.details());
	chatDetailsContact->setState(StorableObject::StateNew);
	chatDetailsContact->setContact(contact);

	if (ActionCreateAndAdd == notFoundAction)
		ChatManager::instance()->addItem(chat);

	return chat;
}

ChatTypeContact::ChatTypeContact(QObject *parent) :
		ChatType(parent)
{
	Aliases.append("Contact");
	Aliases.append("Simple");
}

ChatTypeContact::~ChatTypeContact()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal name of chat type.
 * @return internal name of chat type
 *
 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
 * and also it is stored with @link Chat @endlink data.
 *
 * Internal name for ChatTypeContact is 'Contact'.
 */
QString ChatTypeContact::name() const
{
	return "Contact";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal aliases of chat type.
 * @return internal aliases of chat type
 *
 * Chat type internal aliases, used to imporitng old configurations.
 *
 * Internal aliases for ChatTypeContact are 'Contact' and 'Simple'.
 */
QStringList ChatTypeContact::aliases() const
{
	return Aliases;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Icon of chat type.
 * @return icon of chat type
 *
 * Chat type icon. Icon is used in history window and as icon of chat windows.
 *
 * Icon for ChatTypeContact is 'internet-group-chat'.
 */
KaduIcon ChatTypeContact::icon() const
{
	return KaduIcon("internet-group-chat");
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Window role for this chat type.
 * @return window role for this chat type.
 *
 * Kadu window role for this chat type.
 * For the contact chat the role is "kadu-chat-contact".
 */
QString ChatTypeContact::windowRole() const
{
	return "kadu-chat-contact";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new ChatDetailsContact object for given chat type.
 * @return new ChatDetailsContact object for given chat type
 *
 * Creates new @link ChatDetailsContact @endlink object for
 * given @link Chat @endlink (@link ChatShared @endlink).
 */
ChatDetails * ChatTypeContact::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsContact(chatData);
}

ChatEditWidget * ChatTypeContact::createEditWidget(const Chat &chat, QWidget *parent) const
{
	Q_UNUSED(chat);
	Q_UNUSED(parent);

	return 0;
}
