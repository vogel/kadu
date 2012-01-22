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

#include <QtCore/QFutureWatcher>
#include <QtCore/QModelIndex>

#include "exports.h"

#include "gui/widgets/history-tab.h"

class QStandardItemModel;

class KaduTreeView;
class WaitOverlay;

class KADUAPI SmsHistoryTab : public HistoryTab
{
	Q_OBJECT

	QString CurrentRecipient;

	QFutureWatcher<QVector<QString> > *SmsFutureWatcher;

	KaduTreeView *SmsListView;
	QStandardItemModel *SmsModel;

	void displaySmsRecipient(const QString &recipient, bool force);

private slots:
	void futureSmsAvailable();
	void futureSmsCanceled();

	void showSmsPopupMenu();
	void clearSmsHistory();

	void currentSmsChanged(const QModelIndex &current);

protected:
	virtual void createTreeView(QWidget *parent);
	virtual void displayForDate(const QDate &date);
	virtual void removeEntriesPerDate(const QDate &date);

public:
	explicit SmsHistoryTab(QWidget *parent = 0);
	virtual ~SmsHistoryTab();

	virtual void updateData();

};

#endif // SMS_HISTORY_TAB_H
