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

#include <QtCore/QDate>

#include "gui/widgets/history-tab.h"

class QAbstractButton;
class QCheckBox;
class QDateEdit;
class QLineEdit;
class QRadioButton;
class QSplitter;

class BuddyListModel;
class ChatsListModel;
class SelectTalkableComboBox;
class TimelineChatMessagesView;

class SearchTab : public HistoryTab
{
	Q_OBJECT

	QSplitter *Splitter;
	TimelineChatMessagesView *TimelineView;

	QLineEdit *Query;

	ChatsListModel *ChatsModel;
	BuddyListModel *BuddiesModel;

	QRadioButton *SearchInChats;
	SelectTalkableComboBox *SelectChat;

	BuddyListModel *StatusBuddiesModel;

	QRadioButton *SearchInStatuses;
	SelectTalkableComboBox *SelectStatusBuddy;

	QRadioButton *SearchInSmses;

	QCheckBox *SearchByDate;
	QDateEdit *FromDate;
	QDateEdit *ToDate;

	void createGui();

private slots:
	void kindChanged(QAbstractButton *button);
	void fromDateChanged(const QDate &date);
	void toDateChanged(const QDate &date);
	void performSearch();

	void currentDateChanged();

public:
	explicit SearchTab(QWidget *parent = 0);
	virtual ~SearchTab();

	virtual QList<int> sizes() const;
	virtual void setSizes(const QList<int> &newSizes);

};

#endif // SEARCH_TAB
