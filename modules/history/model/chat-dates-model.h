/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef CHAT_DATES_MODEL
#define CHAT_DATES_MODEL

#include <QtCore/QAbstractListModel>
#include <QtCore/QDate>
#include <QtCore/QList>

#include "chat/chat.h"

class ChatDatesModel : public QAbstractListModel
{
	Q_OBJECT

	struct ItemCachedData
	{
		QString title;
		int size;
	};

	Chat MyChat;
	QList<QDate> Dates;
	QMap<QDate, ItemCachedData> *Cache;

	QString fetchTitle(QDate date) const;
	int fetchSize(QDate date) const;
	ItemCachedData fetchCachedData(QDate date) const;

public:
	ChatDatesModel(Chat chat, QList<QDate> dates, QObject *parent = 0);
	virtual ~ChatDatesModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setChat(Chat chat);
	void setDates(QList<QDate> dates);

};

#endif // CHAT_DATES_MODEL
