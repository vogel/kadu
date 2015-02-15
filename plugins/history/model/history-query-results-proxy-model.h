/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_QUERY_RESULTS_PROXY_MODEL_H
#define HISTORY_QUERY_RESULTS_PROXY_MODEL_H

#include <QtCore/QSortFilterProxyModel>

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryQueryResultsProxyModel
 * @author Rafał 'Vogel' Malinowski
 * @short Model used to filter columns from HistoryQueryResultsModel model.
 *
 * This proxy model is used to hide unneccessary columns from HistoryQueryResultsModel.
 * Use setTalkableVisible() to set visibility of first column and setTitleVisible() to set visibility of
 * fourth column.
 */
class HistoryQueryResultsProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	bool TalkableVisible;
	bool TitleVisible;

protected:
	virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new proxy model.
	 * @param parent QObject parent of new proxy model.
	 */
	explicit HistoryQueryResultsProxyModel(QObject *parent = 0);
	virtual ~HistoryQueryResultsProxyModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set visibility of first (talkable) column.
	 * @param talkableVisible new value of visibility  of first (talkable) column
	 */
	void setTalkableVisible(const bool talkableVisible);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set visibility of fourth (title) column.
	 * @param titleVisible new value of visibility of fourth (title) column
	 */
	void setTitleVisible(const bool titleVisible);

};

/**
 * @}
 */

#endif // HISTORY_QUERY_RESULTS_PROXY_MODEL_H
