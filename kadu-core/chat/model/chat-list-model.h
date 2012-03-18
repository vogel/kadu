/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_LIST_MODEL_H
#define CHAT_LIST_MODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVector>

#include "chat/chat.h"
#include "exports.h"
#include "model/kadu-abstract-model.h"

class KADUAPI ChatListModel : public QAbstractItemModel, public KaduAbstractModel
{
	Q_OBJECT

	QVector<Chat> List;

	void connectChat(const Chat &chat);
	void disconnectChat(const Chat &chat);

	Chat chatFromVariant(const QVariant &variant) const;

private slots:
	void contactAboutToBeAdded(const Contact &contact);
	void contactAdded(const Contact &contact);
	void contactAboutToBeRemoved(const Contact &contact);
	void contactRemoved(const Contact &contact);

	void chatUpdated();

public:
	explicit ChatListModel(QObject *parent = 0);
	virtual ~ChatListModel();

	void setChats(const QVector<Chat> &chats);
	void addChat(const Chat &chat);
	void removeChat(const Chat &chat);

	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QFlags<Qt::ItemFlag> flags(const QModelIndex &index) const;

	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	virtual Chat chatAt(const QModelIndex &index) const;
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

	// D&D
	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList &indexes) const;

};

#endif // CHAT_LIST_MODEL_H
