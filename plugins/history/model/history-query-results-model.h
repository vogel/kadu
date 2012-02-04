/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef HISTORY_QUERY_RESULTS_MODEL_H
#define HISTORY_QUERY_RESULTS_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtCore/QVector>

#include "chat/chat.h"

struct HistoryQueryResult;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryQueryResultsModel
 * @author Rafał 'Vogel' Malinowski
 * @short Model used to represent vector of HistoryQueryResult items.
 *
 * This model can hold a vector of HistoryQueryResult. Four columns are available - talkable
 * name, date, count (length) and title. For each item date and talkable can be obtained using
 * DateRole and TalkableRole values in data() method.
 *
 * Names of first and third column can be changed using setTalkableHeader() and setLengthHeader()
 * methods.
 */
class HistoryQueryResultsModel : public QAbstractListModel
{
	Q_OBJECT

	QString TalkableHeader;
	QString LengthHeader;
	QVector<HistoryQueryResult> Results;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new model.
	 * @param parent QObject parent of new model.
	 */
	explicit HistoryQueryResultsModel(QObject *parent = 0);
	virtual ~HistoryQueryResultsModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set title of first column.
	 * @param talkableHeader new title for first column
	 *
	 * Default title of first column is "Chat".
	 */
	void setTalkableHeader(const QString &talkableHeader);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set title of third column.
	 * @param lengthHeader new title for third column
	 *
	 * Default title of third column is "Length".
	 */
	void setLengthHeader(const QString &lengthHeader);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set vector of HistoryQueryResult items.
	 * @param results new vector of HistoryQueryResult items
	 */
	void setResults(const QVector<HistoryQueryResult> &results);

};

/**
 * @}
 */

#endif // HISTORY_QUERY_RESULTS_MODEL_H
