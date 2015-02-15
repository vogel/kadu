/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "contacts/model/contact-list-model.h"

#include "chat-adapter.h"

ChatAdapter::ChatAdapter(ContactListModel *model, const Chat &chat) :
		QObject(model), Model(model)
{
	Model->setContactList(chat.contacts().toContactVector());

	connect(chat, SIGNAL(contactAdded(Contact)), SLOT(contactAdded(Contact)));
	connect(chat, SIGNAL(contactRemoved(Contact)), SLOT(contactRemoved(Contact)));
}

ChatAdapter::~ChatAdapter()
{
}

void ChatAdapter::contactAdded(const Contact &contact)
{
	Model->addContact(contact);
}

void ChatAdapter::contactRemoved(const Contact &contact)
{
	Model->removeContact(contact);
}

#include "moc_chat-adapter.cpp"
