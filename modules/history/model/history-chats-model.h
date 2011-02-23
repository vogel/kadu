/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_CHATS_MODEL_H
#define HISTORY_CHATS_MODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QMap>

#include "chat/type/chat-type-aware-object.h"

class Buddy;
class Chat;
class ChatType;

class HistoryChatsModel : public QAbstractItemModel, ChatTypeAwareObject
{
	Q_OBJECT

	QList<ChatType *> ChatKeys;
	QList<QList<Chat> > Chats;

	QList<Buddy> StatusBuddies;
	QList<QString> SmsRecipients;

	void clearChats();
	void addChat(const Chat &chat);

	void clearStatusBuddies();
	void clearSmsRecipients();

	QVariant chatTypeData(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant chatData(const QModelIndex &index, int role = Qt::DisplayRole) const;

	QVariant statusData(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant smsRecipientData(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
	virtual void chatTypeRegistered(ChatType *chatType);
	virtual void chatTypeUnregistered(ChatType *chatType);

public:
	HistoryChatsModel(QObject *parent = 0);
	virtual ~HistoryChatsModel();

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setChats(const QList<Chat> &chats);
	void setStatusBuddies(const QList<Buddy> &buddies);
	void setSmsRecipients(const QList<QString> &smsRecipients);

	QModelIndex chatTypeIndex(ChatType *type) const;
	QModelIndex chatIndex(const Chat &chat) const;

	QModelIndex statusIndex() const;
	QModelIndex statusBuddyIndex(const Buddy &buddy) const;

	QModelIndex smsIndex() const;
	QModelIndex smsRecipientIndex(const QString &recipient) const;

};

#endif // HISTORY_CHATS_MODEL_H
