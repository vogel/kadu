/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-style-preview.h"

#include "buddies/buddy-preferred-manager.h"
#include "chat-style/engine/chat-style-renderer-factory-provider.h"
#include "chat/chat-details-contact.h"
#include "core/core.h"
#include "core/myself.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"

#include <QtWidgets/QHBoxLayout>

ChatStylePreview::ChatStylePreview(QWidget *parent) :
		QFrame{parent}
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setFixedHeight(250);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

ChatStylePreview::~ChatStylePreview()
{
}

void ChatStylePreview::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

void ChatStylePreview::setWebkitMessagesViewFactory(WebkitMessagesViewFactory *webkitMessagesViewFactory)
{
	m_webkitMessagesViewFactory = webkitMessagesViewFactory;
}

void ChatStylePreview::init()
{
	auto layout = make_owned<QHBoxLayout>(this);
	layout->setContentsMargins(0, 0, 0, 0);
	m_view = preparePreview();
	layout->addWidget(m_view.get());

	configurationUpdated();
}

void ChatStylePreview::setRendererFactory(std::unique_ptr<ChatStyleRendererFactory> rendererFactory)
{
	m_view.get()->setChatStyleRendererFactory(std::move(rendererFactory));
}

owned_qptr<WebkitMessagesView> ChatStylePreview::preparePreview()
{
	auto example = Buddy::dummy();

	auto chat = Chat::create();
	chat.setType("Contact");

	auto details = static_cast<ChatDetailsContact *>(chat.details());
	details->setState(StorableObject::StateNew);
	details->setContact(Core::instance()->buddyPreferredManager()->preferredContact(example));

	auto buddy = Buddy::create();
	buddy.setDisplay(Core::instance()->myself()->buddy().display());
	auto contact = Contact::create();
	contact.setId("id@network");
	contact.setOwnerBuddy(buddy);

	auto sentMessage = Message::create();
	sentMessage.setMessageChat(chat);
	sentMessage.setType(MessageTypeSent);
	sentMessage.setMessageSender(contact);
	sentMessage.setContent(m_formattedStringFactory->fromPlainText(tr("Your message")));
	sentMessage.setReceiveDate(QDateTime::currentDateTime());
	sentMessage.setSendDate(QDateTime::currentDateTime());

	auto receivedMessage = Message::create();
	receivedMessage.setMessageChat(chat);
	receivedMessage.setType(MessageTypeReceived);
	receivedMessage.setMessageSender(Core::instance()->buddyPreferredManager()->preferredContact(example));
	receivedMessage.setContent(m_formattedStringFactory->fromPlainText(tr("Message from Your friend")));
	receivedMessage.setReceiveDate(QDateTime::currentDateTime());
	receivedMessage.setSendDate(QDateTime::currentDateTime());

	auto result = m_webkitMessagesViewFactory->createWebkitMessagesView(chat, false, this);
	result->add(sentMessage);
	result->add(receivedMessage);
	return std::move(result);
}

void ChatStylePreview::configurationUpdated()
{
	m_view->setUserFont(Core::instance()->chatConfigurationHolder()->chatFont().toString(), Core::instance()->chatConfigurationHolder()->forceCustomChatFont());
}

#include "moc_chat-style-preview.cpp"
