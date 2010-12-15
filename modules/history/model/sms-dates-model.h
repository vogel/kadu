/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_DATES_MODEL_H
#define SMS_DATES_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QDate>
#include <QtCore/QList>

#include "buddies/buddy.h"

class SmsDatesModel : public QAbstractListModel
{
	Q_OBJECT

	struct ItemCachedData
	{
		int size;
	};

	QString Recipient;
	QList<QDate> Dates;
	QMap<QDate, ItemCachedData> *Cache;

	int fetchSize(const QDate &date) const;
	ItemCachedData fetchCachedData(const QDate &date) const;

public:
	SmsDatesModel(const QString &recipient, const QList<QDate> &dates, QObject *parent = 0);
	virtual ~SmsDatesModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setRecipient(const QString &recipient);
	void setDates(const QList<QDate> &dates);

	QModelIndex indexForDate(const QDate &date);

};

#endif // SMS_DATES_MODEL_H
