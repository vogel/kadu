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

#pragma once

#include "chat-style/engine/chat-style-renderer-factory.h"
#include "configuration/configuration-aware-object.h"
#include "misc/memory.h"

#include <QtCore/QPointer>
#include <QtWidgets/QFrame>
#include <injeqt/injeqt.h>

class BuddyPreferredManager;
class BuddyStorage;
class ChatConfigurationHolder;
class ChatStorage;
class ContactStorage;
class FormattedStringFactory;
class MessageStorage;
class Myself;
class WebkitMessagesViewFactory;
class WebkitMessagesView;

class ChatStylePreview : public QFrame, public ConfigurationAwareObject
{
	Q_OBJECT

public:
	explicit ChatStylePreview(QWidget *parent = nullptr);
	virtual ~ChatStylePreview();

	void setRendererFactory(std::unique_ptr<ChatStyleRendererFactory> rendererFactory);

protected:
	virtual void configurationUpdated();

private:
	QPointer<BuddyPreferredManager> m_buddyPreferredManager;
	QPointer<BuddyStorage> m_buddyStorage;
	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<ContactStorage> m_contactStorage;
	QPointer<FormattedStringFactory> m_formattedStringFactory;
	QPointer<MessageStorage> m_messageStorage;
	QPointer<Myself> m_myself;
	QPointer<WebkitMessagesViewFactory> m_webkitMessagesViewFactory;
	owned_qptr<WebkitMessagesView> m_view;

	owned_qptr<WebkitMessagesView> preparePreview();

private slots:
	INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setContactStorage(ContactStorage *contactStorage);
	INJEQT_SET void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);
	INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_SET void setWebkitMessagesViewFactory(WebkitMessagesViewFactory *webkitMessagesViewFactory);
	INJEQT_INIT void init();

};
