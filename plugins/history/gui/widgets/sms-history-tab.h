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

#ifndef SMS_HISTORY_TAB_H
#define SMS_HISTORY_TAB_H

#include <QtCore/QModelIndex>

#include "exports.h"

#include "gui/widgets/history-tab.h"

class QMenu;
class QStandardItemModel;

class HistoryDatesModel;
class KaduTreeView;
class TimelineChatMessagesView;

class KADUAPI SmsHistoryTab : public HistoryTab
{
	Q_OBJECT

	KaduTreeView *SmsListView;
	QStandardItemModel *SmsModel;

	QMenu *SmsDetailsPopupMenu;
	HistoryDatesModel *MySmsDatesModel;

	void smsRecipientActivated(const QString &recipient);

private slots:
	void currentSmsChanged(const QModelIndex &current, const QModelIndex &previous);
	void smsDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
	void showSmsPopupMenu(const QPoint &pos);
	void clearSmsHistory();
	void removeSmsEntriesPerDate();

protected:
	virtual void createTreeView(QWidget *parent);

protected slots:
	virtual void showTimelinePopupMenu(const QPoint &pos);

public:
	explicit SmsHistoryTab(QWidget *parent = 0);
	virtual ~SmsHistoryTab();

	virtual void updateData();

};

#endif // SMS_HISTORY_TAB_H
