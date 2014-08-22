/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-html-visitor.h"
#include "formatted-string/formatted-string-plain-text-visitor.h"
#include "formatted-string/formatted-string.h"
#include "message/message-manager.h"
#include "message/message.h"
#include "message/unread-message-repository.h"
#include "misc/change-notifier.h"

#include "message-shared.h"

MessageShared * MessageShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint)
{
	MessageShared *result = loadFromStorage(messageStoragePoint);
	result->setFormattedStringFactory(Core::instance()->formattedStringFactory());
	result->loadStub();
	return result;
}

MessageShared * MessageShared::loadFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint)
{
	MessageShared *result = new MessageShared();
	result->setFormattedStringFactory(Core::instance()->formattedStringFactory());
	result->setStorage(messageStoragePoint);
	return result;
}

MessageShared::MessageShared(const QUuid &uuid) :
		Shared(uuid), Status(MessageStatusUnknown), Type(MessageTypeUnknown)
{
	MessageChat = new Chat();
	MessageSender = new Contact();

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

MessageShared::~MessageShared()
{
	ref.ref();

	delete MessageSender;
	delete MessageChat;
}

void MessageShared::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

StorableObject * MessageShared::storageParent()
{
	return Core::instance()->unreadMessageRepository();
}

QString MessageShared::storageNodeName()
{
	return QLatin1String("Message");
}

void MessageShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	*MessageChat = ChatManager::instance()->byUuid(loadValue<QString>("Chat"));
	*MessageSender = ContactManager::instance()->byUuid(loadValue<QString>("Sender"));

	if (CurrentFormattedStringFactory)
		setContent(CurrentFormattedStringFactory.data()->fromHtml(loadValue<QString>("Content")));

	ReceiveDate = loadValue<QDateTime>("ReceiveDate");
	SendDate = loadValue<QDateTime>("SendDate");
	Status = (MessageStatus)loadValue<int>("Status");
	Type = (MessageType)loadValue<int>("Type");
	Id = loadValue<QString>("Id");
}

void MessageShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Chat", MessageChat->uuid().toString());
	storeValue("Sender", MessageSender->uuid().toString());
	storeValue("Content", HtmlContent);
	storeValue("ReceiveDate", ReceiveDate);
	storeValue("SendDate", SendDate);
	storeValue("Status", (int)Status);
	storeValue("Type", (int)Type);
	storeValue("Id", Id);
}

bool MessageShared::shouldStore()
{
	ensureLoaded();

	// only store pending messages
	// all other messages are stored by history plugin
	return UuidStorableObject::shouldStore()
			&& !MessageSender->uuid().isNull()
			&& !MessageChat->uuid().isNull();
}

void MessageShared::setStatus(MessageStatus status)
{
	ensureLoaded();

	if (status != Status)
	{
		MessageStatus oldStatus = Status;
		Status = status;
		changeNotifier().notify();
		emit statusChanged(oldStatus);
	}
}

void MessageShared::setContent(std::unique_ptr<FormattedString> &&content)
{
	Content = std::move(content);

	if (!Content)
	{
		PlainTextContent.clear();
		HtmlContent.clear();
	}
	else
	{
		FormattedStringPlainTextVisitor plainTextVisitor;
		FormattedStringHtmlVisitor htmlVisitor;

		Content->accept(&plainTextVisitor);
		Content->accept(&htmlVisitor);

		PlainTextContent = plainTextVisitor.result();
		HtmlContent = htmlVisitor.result();
	}
}

FormattedString * MessageShared::content() const
{
	return Content.get();
}

KaduShared_PropertyPtrDefCRW(MessageShared, Chat, messageChat, MessageChat)
KaduShared_PropertyPtrDefCRW(MessageShared, Contact, messageSender, MessageSender)

#include "moc_message-shared.cpp"
