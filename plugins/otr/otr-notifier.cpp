/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "chat/type/chat-type-contact.h"
#include "notify/notify-event.h"
#include "notify/notification-manager.h"
#include "notify/notification/chat-notification.h"

#include "otr-notifier.h"

QString OtrNotifier::OtrNotifyTopic("OTR");
QString OtrNotifier::TryingToStartSessionNotifyTopic("OTR/TryingToStartSession");
QString OtrNotifier::PeerClosedSessionNotifyTopic("OTR/PeerClosedStartSession");
QString OtrNotifier::GoneSecureNotifyTopic("OTR/GoneSecure");
QString OtrNotifier::GoneInsecureNotifyTopic("OTR/GoneInsecure");
QString OtrNotifier::StillSecureNotifyTopic("OTR/StillSecure");
QString OtrNotifier::CreatePrivateKeyStartedNotifyTopic("OTR/CreatePrivateKeyStarted");
QString OtrNotifier::CreatePrivateKeyFinishedNotifyTopic("OTR/CreatePrivateKeyFinished");

OtrNotifier::OtrNotifier(QObject *parent) :
		QObject(parent)
{
	OtrNotifyEvent.reset(new NotifyEvent(OtrNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "OTR Encryption")));
	TryingToStartSessionNotifyEvent.reset(new NotifyEvent(TryingToStartSessionNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Trying to start private conversation")));
	PeerClosedSessionNotifyEvent.reset(new NotifyEvent(PeerClosedSessionNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Peer closed private conversation")));
	GoneSecureNotifyEvent.reset(new NotifyEvent(GoneSecureNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Conversation gone secure")));
	GoneInsecureNotifyEvent.reset(new NotifyEvent(GoneInsecureNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Conversation gone insecure")));
	StillSecureNotifyEvent.reset(new NotifyEvent(StillSecureNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Conversation still secure")));
	CreatePrivateKeyStartedNotifyEvent.reset(new NotifyEvent(CreatePrivateKeyStartedNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Create private key started")));
	CreatePrivateKeyFinishedNotifyEvent.reset(new NotifyEvent(CreatePrivateKeyFinishedNotifyTopic, NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "Create private key finished")));
}

OtrNotifier::~OtrNotifier()
{
}

QList<NotifyEvent *> OtrNotifier::notifyEvents()
{
	return QList<NotifyEvent *>()
			<< OtrNotifyEvent.data()
			<< TryingToStartSessionNotifyEvent.data()
			<< GoneSecureNotifyEvent.data()
			<< GoneInsecureNotifyEvent.data()
			<< StillSecureNotifyEvent.data()
			<< CreatePrivateKeyStartedNotifyEvent.data()
			<< CreatePrivateKeyFinishedNotifyEvent.data();
}

void OtrNotifier::notify(const QString &topic, const Account &account, const QString &message)
{
	AccountNotification *notification = new AccountNotification(account, topic, KaduIcon());
	notification->setTitle(tr("OTR Encryption"));
	notification->setText(message);

	NotificationManager::instance()->notify(notification);
}

void OtrNotifier::notify(const QString &topic, const Contact &contact, const QString &message)
{
	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
	ChatNotification *notification = new ChatNotification(chat, topic, KaduIcon());
	notification->setTitle(tr("OTR Encryption"));
	notification->setText(message);

	NotificationManager::instance()->notify(notification);
}

void OtrNotifier::notifyTryingToStartSession(const Contact &contact)
{
	notify(TryingToStartSessionNotifyTopic, contact,
		   tr("%1: trying to start private conversation").arg(contact.display(true)));
}

void OtrNotifier::notifyPeerEndedSession(const Contact &contact)
{
	notify(PeerClosedSessionNotifyTopic, contact,
		   tr("%1: peer ended private conversation; you should do the same").arg(contact.display(true)));
}

void OtrNotifier::notifyGoneSecure(const Contact &contact)
{
	notify(GoneSecureNotifyTopic, contact, tr("%1: private conversation started").arg(contact.display(true)));
}

void OtrNotifier::notifyGoneInsecure(const Contact &contact)
{
	notify(GoneInsecureNotifyTopic, contact, tr("%1: private conversation stopped").arg(contact.display(true)));
}

void OtrNotifier::notifyStillSecure(const Contact &contact)
{
	notify(StillSecureNotifyTopic, contact, tr("%1: conversation is still private").arg(contact.display(true)));
}

void OtrNotifier::notifyCreatePrivateKeyStarted(const Account &account)
{
	notify(CreatePrivateKeyStartedNotifyTopic, account,
		   tr("%1: creating private key, it can took a few minutes").arg(account.id()));
}

void OtrNotifier::notifyCreatePrivateKeyFinished(const Account &account, bool ok)
{
	notify(CreatePrivateKeyFinishedNotifyTopic, account, ok
			? tr("%1: private key created, you can start a private conversation now").arg(account.id())
			: tr("%1: private key creation failed").arg(account.id()));
}
