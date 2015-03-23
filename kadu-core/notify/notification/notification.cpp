/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QTimer>

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "notify/notification-manager.h"
#include "notify/notifier.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "debug.h"

#include "notification.h"

static QString getAccountName(const ParserData * const object)
{
	const Notification * const notification = dynamic_cast<const Notification * const>(object);
	return notification && !notification->account().isNull()
			? notification->account().accountIdentity().name()
			: QString();
}

static QString getProtocolName(const ParserData * const object)
{
	const Notification * const notification = dynamic_cast<const Notification * const>(object);
	return notification &&
			!notification->account().isNull() &&
			notification->account().protocolHandler() &&
			notification->account().protocolHandler()->protocolFactory()
		? notification->account().protocolHandler()->protocolFactory()->displayName()
		: QString();
}

static QString getNotificationTitle(const ParserData * const object)
{
	kdebugf();

	const Notification * const notification = dynamic_cast<const Notification * const>(object);
	if (notification)
		return notification->title();
	else
		return QString();
}

void Notification::registerParserTags()
{
	Parser::registerObjectTag("account", getAccountName);
	Parser::registerObjectTag("event", getNotificationTitle);
	Parser::registerObjectTag("protocol", getProtocolName);
}

void Notification::unregisterParserTags()
{
	Parser::unregisterObjectTag("account");
	Parser::unregisterObjectTag("event");
	Parser::unregisterObjectTag("protocol");
}

Notification::Notification(Account account, Chat chat, const QString &type, const KaduIcon &icon) :
		Type(type),
		Icon(icon),
		m_account{account},
		m_chat{chat},
		DefaultCallbackTimer(0),
		Closing(false)
{
}

Notification::~Notification()
{
}

Account Notification::account() const
{
	return m_account;
}

Chat Notification::chat() const
{
	return m_chat;
}

void Notification::acquire(Notifier *notifier)
{
	kdebugf();

	Notifiers.insert(notifier);
}

void Notification::release(Notifier *notifier)
{
	kdebugf();

	Notifiers.remove(notifier);

	if (Notifiers.size() <= 0)
		close();
}

void Notification::close()
{
	kdebugf();

	if (!Closing)
	{
		Closing = true;
		emit closed(this);
		deleteLater();
	}
}

void Notification::clearCallbacks()
{
	Callbacks.clear();
}

void Notification::addCallback(const QString &caption, const char *slot, const char *signature)
{
	auto callback = NotificationCallback{caption, slot, signature};
	Callbacks.append(callback);
}

void Notification::addChatCallbacks()
{
	addCallback(tr("Chat"), SLOT(callbackAccept()), "callbackAccept()");
	addCallback(tr("Ignore"), SLOT(callbackDiscard()), "callbackDiscard()");
}

void Notification::setDefaultCallback(int timeout, const char *defaultSlot)
{
	DefaultCallbackTimer = new QTimer(this);
	DefaultCallbackTimer->setSingleShot(true);
	connect(DefaultCallbackTimer, SIGNAL(timeout()), this, defaultSlot);
	DefaultCallbackTimer->start(timeout);
}

void Notification::callbackAccept()
{
	close();

	if (m_chat)
		Core::instance()->chatWidgetManager()->openChat(m_chat, OpenChatActivation::Activate);
}

void Notification::callbackDiscard()
{
	close();
}

void Notification::clearDefaultCallback()
{
	if (DefaultCallbackTimer)
	{
		delete DefaultCallbackTimer;
		DefaultCallbackTimer = 0;
	}
}

QString Notification::key() const
{
	return NotificationManager::instance()->notifyConfigurationKey(Type);
}

QString Notification::groupKey() const
{
	if (m_chat)
		return m_chat.uuid().toString();
	else
		return Title;
}

void Notification::setTitle(const QString &title)
{
	Title = title;
}

void Notification::setText(const QString &text)
{
	Text = text;
}

void Notification::setDetails(const QStringList &details)
{
	Details = details;
}

void Notification::setDetails(const QString &details)
{
	Details = QStringList(details);
}

void Notification::setIcon(const KaduIcon &icon)
{
	Icon = icon;
}

#include "moc_notification.cpp"
