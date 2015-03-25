/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-title.h"

#include "chat/type/chat-type-manager.h"
#include "contacts/contact-set.h"
#include "contacts/model/contact-data-extractor.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/chat-widget/chat-widget-title-composing-state-position.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "protocols/services/chat-state.h"

ChatWidgetTitle::ChatWidgetTitle(ChatWidget *parent) :
		QObject{parent},
		m_composingStatePosition{ChatWidgetTitleComposingStatePosition::None},
		m_showUnreadMessagesCount{false},
		m_blinkTitleWhenUnreadMessages{false},
		m_blinkIconWhenUnreadMessages{false},
		m_blink{false}
{
	connect(parent->chat(), SIGNAL(updated()), this, SLOT(update()));
	for (auto &&contact : parent->chat().contacts())
	{
		connect(contact, SIGNAL(updated()), this, SLOT(update()));
		if (contact.ownerBuddy())
			connect(contact.ownerBuddy(), SIGNAL(updated()), this, SLOT(update()));
	}

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(update()));
}

ChatWidgetTitle::~ChatWidgetTitle()
{
}

ChatWidget * ChatWidgetTitle::chatWidget() const
{
	return static_cast<ChatWidget *>(parent());
}

QString ChatWidgetTitle::title() const
{
	return m_blinkTimer && m_blink && m_blinkTitleWhenUnreadMessages
			? QString{" "} // do not emit empty title
			: m_title;
}

QString ChatWidgetTitle::shortTitle() const
{
	return m_title.length() > 15
			? m_title.left(15) + "..."
			: m_title;
}

QString ChatWidgetTitle::fullTitle() const
{
	return m_fullTitle;
}

QString ChatWidgetTitle::blinkingFullTitle() const
{
	return m_blinkTimer && m_blink && m_blinkTitleWhenUnreadMessages
			? QString{" "} // do not emit empty title
			: m_fullTitle;
}

QString ChatWidgetTitle::tooltip() const
{
	return m_tooltip;
}

QIcon ChatWidgetTitle::icon() const
{
	return chatWidget()->chat().unreadMessagesCount() > 0
			? KaduIcon("protocols/common/message").icon()
			: m_icon;
}

QIcon ChatWidgetTitle::blinkingIcon() const
{
	auto showMessageIcon = m_blinkTimer && m_blinkIconWhenUnreadMessages
			? m_blink
			: chatWidget()->chat().unreadMessagesCount() > 0;
	return showMessageIcon
			? KaduIcon("protocols/common/message").icon()
			: m_icon;
}

void ChatWidgetTitle::setComposingStatePosition(ChatWidgetTitleComposingStatePosition composingStatePosition)
{
	if (m_composingStatePosition == composingStatePosition)
		return;

	m_composingStatePosition = composingStatePosition;
	if (m_composingStatePosition == ChatWidgetTitleComposingStatePosition::None)
		disconnect(chatWidget(), SIGNAL(chatStateChanged(ChatState)), this, SLOT(update()));
	else
		connect(chatWidget(), SIGNAL(chatStateChanged(ChatState)), this, SLOT(update()));

	update();
}

void ChatWidgetTitle::setShowUnreadMessagesCount(bool showUnreadMessagesCount)
{
	if (m_showUnreadMessagesCount == showUnreadMessagesCount)
		return;

	m_showUnreadMessagesCount = showUnreadMessagesCount;
	update();
}

void ChatWidgetTitle::setBlinkTitleWhenUnreadMessages(bool blinkTitleWhenUnreadMessages)
{
	if (m_blinkTitleWhenUnreadMessages == blinkTitleWhenUnreadMessages)
		return;

	m_blinkTitleWhenUnreadMessages = blinkTitleWhenUnreadMessages;
	update();
}

void ChatWidgetTitle::setBlinkIconWhenUnreadMessages(bool blinkIconWhenUnreadMessages)
{
	if (m_blinkIconWhenUnreadMessages == blinkIconWhenUnreadMessages)
		return;

	m_blinkIconWhenUnreadMessages = blinkIconWhenUnreadMessages;
	update();
}

void ChatWidgetTitle::startBlinking()
{
	if (m_blinkTimer)
		return;

	m_blink = false;
	m_blinkTimer = new QTimer{this};
	connect(m_blinkTimer, SIGNAL(timeout()), this, SLOT(blink()));
	m_blinkTimer->start(500);
}

void ChatWidgetTitle::stopBlinking()
{
	if (!m_blinkTimer)
		return;

	m_blink = false;
	m_blinkTimer->stop();
	m_blinkTimer->deleteLater();
	m_blinkTimer = nullptr;
}

void ChatWidgetTitle::blink()
{
	m_blink = !m_blink;
	update();
}

void ChatWidgetTitle::update()
{
	if ((m_blinkTitleWhenUnreadMessages || m_blinkIconWhenUnreadMessages) && chatWidget()->chat().unreadMessagesCount() > 0)
		startBlinking();
	else
		stopBlinking();

	auto chat = chatWidget()->chat();
	m_title = chatTitle(chat);
	m_fullTitle = withDescription(chat, m_title);
	m_tooltip = cleanUp(m_fullTitle);
	m_title = withCompositionInfo(m_title);
	m_fullTitle = withCompositionInfo(m_fullTitle);
	m_title = withUnreadMessagesCount(m_title);
	m_fullTitle = withUnreadMessagesCount(m_fullTitle);
	m_title = cleanUp(m_title);
	m_fullTitle = cleanUp(m_fullTitle);
	m_icon = chatIcon(chat);

	emit titleChanged(chatWidget());
}

QString ChatWidgetTitle::chatTitle(const Chat &chat) const
{
	if (!chat.display().isEmpty())
		return chat.display();

	auto contactsCount = chat.contacts().count();
	if (contactsCount > 1)
		return tr("Conference [%1]").arg(contactsCount);
	else
		return chat.name();
}

QString ChatWidgetTitle::withDescription(const Chat &chat, const QString &title) const
{
	auto contact = chat.contacts().toContact();
	if (!contact)
		return title;
	if (contact.currentStatus().description().isEmpty())
		return title;
	return QString{"%1 (%2)"}.arg(title).arg(contact.currentStatus().description());
}

QString ChatWidgetTitle::withCompositionInfo(const QString &title) const
{
	auto addon = QString{};
	switch (chatWidget()->chatState())
	{
		case ChatState::Composing:
			addon = tr("(Composing...)");
			break;
		case ChatState::Inactive:
			addon = tr("(Inactive)");
			break;
		default:
			break;
	}

	if (addon.isEmpty())
		return title;

	switch (m_composingStatePosition)
	{
		case ChatWidgetTitleComposingStatePosition::AtBegining:
			return QString{"%1 %2"}.arg(addon).arg(title);
		case ChatWidgetTitleComposingStatePosition::AtEnd:
			return QString{"%1 %2"}.arg(title).arg(addon);
		default:
			return title;
	}
}

QString ChatWidgetTitle::withUnreadMessagesCount(QString title) const
{
	if (!m_showUnreadMessagesCount)
		return title;

	auto count = chatWidget()->chat().unreadMessagesCount();
	return count > 0
			? QString{"[%1] %2"}.arg(count).arg(title)
			: title;
}

QString ChatWidgetTitle::cleanUp(QString title) const
{
	title.replace("\n", " ");
	title.replace("\r", " ");
	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");
	return title;
}

QIcon ChatWidgetTitle::chatIcon(const Chat& chat) const
{
	auto contactsCount = chat.contacts().count();
	if (contactsCount == 1)
	{
		auto contact = chat.contacts().toContact();
		if (contact)
			return ContactDataExtractor::data(contact, Qt::DecorationRole, false).value<QIcon>();
	}
	else if (contactsCount > 1)
		return ChatTypeManager::instance()->chatType("ContactSet")->icon().icon();

	return KaduIcon("internet-group-chat").icon();
}
