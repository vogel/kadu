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

#ifndef SEARCH_TAB
#define SEARCH_TAB

#include "gui/widgets/history-tab.h"

class QLineEdit;
class QSplitter;

class HistoryDatesModel;
class TimelineChatMessagesView;

class SearchTab : public HistoryTab
{
	Q_OBJECT

	QSplitter *Splitter;
	QLineEdit *Query;
	TimelineChatMessagesView *TimelineView;
	HistoryDatesModel *DatesModel;

	void createGui();

private slots:
	void performSearch();

public:
	explicit SearchTab(QWidget *parent = 0);
	virtual ~SearchTab();

	virtual QList<int> sizes() const;
	virtual void setSizes(const QList<int> &newSizes);

};

#endif // SEARCH_TAB
