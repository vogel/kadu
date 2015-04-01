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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QtCore/QPair>

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "icons/kadu-icon.h"
#include "parser/parser-data.h"
#include "exports.h"

class QTimer;

class Notifier;

class KADUAPI Notification : public QObject, public ParserData
{
	Q_OBJECT

	QVariantMap m_data;

	QString Type;

	QString Title;
	QString Text;
	QStringList Details;
	KaduIcon Icon;

	Account m_account;
	Chat m_chat;

	QList<QString> Callbacks;

protected:
	QSet<Notifier *> Notifiers;
	bool Closing;

public:
	static void registerParserTags();
	static void unregisterParserTags();

	Notification(QVariantMap data, const QString &type, const KaduIcon &icon);
	Notification(Account account, Chat chat, const QString &type, const KaduIcon &icon);
	virtual ~Notification();

	const QVariantMap & data() const;

	virtual void acquire(Notifier *notifier);
	virtual void release(Notifier *notifier);
	virtual void close();

	void clearCallbacks();
	void addCallback(const QString &name);
	void addChatCallbacks();

	virtual const QString & type() const { return Type; }

	virtual QString key() const;

	virtual QString groupKey() const;

	virtual QString identifier() { return Type + "_" + groupKey(); }

	void setTitle(const QString &title);

	virtual const QString title() const { return Title; }
	virtual void setText(const QString &text);
	virtual const QString text() const { return Text; }
	virtual void setDetails(const QStringList &details);
	virtual void setDetails(const QString &details);
	virtual const QStringList details() const { return Details; }

	virtual bool isPeriodic() { return false; }
	virtual int period() { return 0; }
	virtual void setIcon(const KaduIcon &icon);
	virtual const KaduIcon & icon() const { return Icon; }
	virtual const QList<QString> & getCallbacks() { return Callbacks; }

public slots:
	virtual void callbackAccept();
	virtual void callbackDiscard();

signals:
	void updated(Notification *);
	void closed(Notification *);


};

#endif // NOTIFICATION_H
