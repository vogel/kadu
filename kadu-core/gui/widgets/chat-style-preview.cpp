/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "chat/chat-details-contact.h"
#include "chat/html-messages-renderer.h"
#include "configuration/chat-configuration-holder.h"
#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/widgets/kadu-web-view.h"

#include <QtGui/QHBoxLayout>

ChatStylePreview::ChatStylePreview(QWidget *parent) :
		QFrame{parent}
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setFixedHeight(250);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	m_htmlMessagesRenderer = make_qobject<HtmlMessagesRenderer>(Chat::null, this);

	auto layout = make_qobject<QHBoxLayout>(this);
	layout->setContentsMargins(0, 0, 0, 0);

	m_view = make_qobject<KaduWebView>(this);
	m_view->setPage(m_htmlMessagesRenderer->webPage());
	m_view->setImageStorageService(Core::instance()->imageStorageService());
	layout->addWidget(m_view.get());

	auto p = palette();
	p.setBrush(QPalette::Base, Qt::transparent);
	m_view->page()->setPalette(p);
	m_view->setAttribute(Qt::WA_OpaquePaintEvent, false);

	preparePreview();
	configurationUpdated();
}

ChatStylePreview::~ChatStylePreview()
{
}

void ChatStylePreview::setRenderer(std::unique_ptr<ChatMessagesRenderer> renderer)
{
	m_messagesRenderer = std::move(renderer);
	m_messagesRenderer.get()->clearMessages(m_htmlMessagesRenderer.get());
	m_messagesRenderer.get()->refreshView(m_htmlMessagesRenderer.get());
}

void ChatStylePreview::preparePreview()
{
	auto example = Buddy::dummy();

	auto chat = Chat::create();
	chat.setType("Contact");

	auto details = dynamic_cast<ChatDetailsContact *>(chat.details());
	details->setState(StorableObject::StateNew);
	details->setContact(BuddyPreferredManager::instance()->preferredContact(example));

	auto buddy = Buddy::create();
	buddy.setDisplay(Core::instance()->myself().display());
	auto contact = Contact::create();
	contact.setId("id@network");
	contact.setOwnerBuddy(buddy);

	auto sentMessage = Message::create();
	sentMessage.setMessageChat(chat);
	sentMessage.setType(MessageTypeSent);
	sentMessage.setMessageSender(contact);
	sentMessage.setContent(Core::instance()->formattedStringFactory()->fromPlainText(tr("Your message")));
	sentMessage.setReceiveDate(QDateTime::currentDateTime());
	sentMessage.setSendDate(QDateTime::currentDateTime());

	auto receivedMessage = Message::create();
	receivedMessage.setMessageChat(chat);
	receivedMessage.setType(MessageTypeReceived);
	receivedMessage.setMessageSender(BuddyPreferredManager::instance()->preferredContact(example));
	receivedMessage.setContent(Core::instance()->formattedStringFactory()->fromPlainText(tr("Message from Your friend")));
	receivedMessage.setReceiveDate(QDateTime::currentDateTime());
	receivedMessage.setSendDate(QDateTime::currentDateTime());

	m_htmlMessagesRenderer->setChat(chat);
	m_htmlMessagesRenderer->appendMessage(sentMessage);
	m_htmlMessagesRenderer->appendMessage(receivedMessage);
}

void ChatStylePreview::configurationUpdated()
{
	m_view->setUserFont(ChatConfigurationHolder::instance()->chatFont().toString(), ChatConfigurationHolder::instance()->forceCustomChatFont());
}

#include "moc_chat-style-preview.cpp"
