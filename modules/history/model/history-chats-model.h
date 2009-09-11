/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HISTORY_CHATS_MODEL_H
#define HISTORY_CHATS_MODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QMap>

class Chat;
class ChatType;

class HistoryChatsModel : public QAbstractItemModel
{
	Q_OBJECT

	QMap<ChatType, QList<Chat *> > Chats;

	QVariant chatTypeData(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant chatData(const QModelIndex &index, int role = Qt::DisplayRole) const;

public:
	HistoryChatsModel(QObject *parent = 0);
	virtual ~HistoryChatsModel();

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void clear();
	void addChat(Chat *chat);

	QModelIndex chatTypeIndex(ChatType type) const;

};

#endif // HISTORY_CHATS_MODEL_H
