/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "icons/kadu-icon.h"
#include "parser/parser-data.h"
#include "exports.h"

#include <QtCore/QPair>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatWidgetManager;
class NotificationCallbackRepository;
class NotificationManager;
class Notifier;
class Parser;

class QTimer;

class KADUAPI Notification : public QObject, public ParserData
{
	Q_OBJECT

public:
	static void registerParserTags(Parser *parser);
	static void unregisterParserTags(Parser *parser);

	Notification(QVariantMap data, const QString &type, const KaduIcon &icon);
	Notification(Account account, Chat chat, const QString &type, const KaduIcon &icon);
	virtual ~Notification();

	const QVariantMap & data() const;

	void acquire(Notifier *notifier);
	void release(Notifier *notifier);
	void close();

	void clearCallbacks();
	void addCallback(const QString &name);
	void addChatCallbacks();

	void setAcceptCallback(QString acceptCallback);
	void setDiscardCallback(QString discardCallback);

	const QString & type() const { return Type; }

	QString key() const;

	void setTitle(const QString &title);

	const QString title() const { return Title; }
	void setText(const QString &text);
	const QString text() const { return Text; }
	void setDetails(const QStringList &details);
	void setDetails(const QString &details);
	const QStringList details() const { return Details; }

	void setIcon(const KaduIcon &icon);
	const KaduIcon & icon() const { return Icon; }
	const QList<QString> & getCallbacks() { return Callbacks; }

public slots:
	virtual void callbackAccept();
	virtual void callbackDiscard();

signals:
	void updated(Notification *);
	void closed(Notification *);

protected:
	QSet<Notifier *> Notifiers;
	bool Closing;

private:
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;
	QPointer<NotificationManager> m_notificationManager;

	QVariantMap m_data;
	QString m_acceptCallback;
	QString m_discardCallback;

	QString Type;

	QString Title;
	QString Text;
	QStringList Details;
	KaduIcon Icon;

	Account m_account;
	Chat m_chat;

	QList<QString> Callbacks;

private slots:
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);
	INJEQT_SET void setNotificationManager(NotificationManager *notificationManager);

};
