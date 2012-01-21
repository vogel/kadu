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

#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QSplitter>
#include <QtGui/QStandardItemModel>
#include <QtGui/QVBoxLayout>

#include "model/roles.h"

#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/kadu-tree-view.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "model/dates-model-item.h"
#include "search/history-search-parameters.h"
#include "history.h"

#include "sms-history-tab.h"

SmsHistoryTab::SmsHistoryTab(QWidget *parent) :
		HistoryTab(false, parent), SmsFutureWatcher(0)
{
	createGui();
}

SmsHistoryTab::~SmsHistoryTab()
{
}

void SmsHistoryTab::createTreeView(QWidget *parent)
{
	FilteredTreeView *smsListWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);
	smsListWidget->filterWidget()->setAutoVisibility(false);
	smsListWidget->filterWidget()->setLabel(tr("Filter") + ":");

	SmsListView = new KaduTreeView(smsListWidget);
	SmsListView->setAlternatingRowColors(true);
	SmsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	SmsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	SmsModel = new QStandardItemModel(SmsListView);
	QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(SmsModel);
	proxyModel->setSourceModel(SmsModel);

	connect(smsListWidget, SIGNAL(filterChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));

	SmsListView->setModel(proxyModel);

	connect(SmsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(currentSmsChanged(QModelIndex)));
	connect(SmsListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSmsPopupMenu()));
	SmsListView->setContextMenuPolicy(Qt::CustomContextMenu);

	smsListWidget->setView(SmsListView);
}

void SmsHistoryTab::displaySmsRecipient(const QString& recipient, bool force)
{
	if (!force && CurrentRecipient == recipient)
		return;

	Chat smsChat = Chat::create();
	smsChat.setDisplay(recipient);
	timelineView()->messagesView()->setChat(smsChat);

	CurrentRecipient = recipient;
	setFutureDates(History::instance()->datesForSmsRecipient(CurrentRecipient));
}

void SmsHistoryTab::showSmsPopupMenu()
{
	QScopedPointer<QMenu> menu;

	menu.reset(new QMenu(this));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear SMS History"), this, SLOT(clearSmsHistory()));

	menu->exec(QCursor::pos());
}

void SmsHistoryTab::clearSmsHistory()
{
	bool removed = false;

	const QModelIndexList &indexes = SmsListView->selectionModel()->selectedIndexes();
	foreach (const QModelIndex &index, indexes)
	{
		QString recipient = index.data(Qt::DisplayRole).toString();
		if (recipient.isEmpty())
			continue;

		removed = true;
		History::instance()->currentStorage()->clearSmsHistory(recipient);
	}

	if (removed)
	{
		updateData();
		displaySmsRecipient(QString(), false);
	}
}

void SmsHistoryTab::currentSmsChanged(const QModelIndex &current)
{
	displaySmsRecipient(current.data().toString(), false);
}

void SmsHistoryTab::displayForDate(const QDate &date)
{
	if (!CurrentRecipient.isEmpty() && date.isValid())
		setFutureMessages(History::instance()->sms(CurrentRecipient, date));
}

void SmsHistoryTab::removeEntriesPerDate(const QDate &date)
{
	if (!CurrentRecipient.isEmpty())
	{
		History::instance()->currentStorage()->clearSmsHistory(CurrentRecipient, date);
		displaySmsRecipient(CurrentRecipient, true);
	}
}

void SmsHistoryTab::futureSmsAvailable()
{
	hideTabWaitOverlay();

	if (!SmsFutureWatcher)
		return;

	QList<QString> smsRecipients = SmsFutureWatcher->result();

	SmsModel->clear();
	foreach (const QString &smsRecipient, smsRecipients)
		SmsModel->appendRow(new QStandardItem(KaduIcon("phone").icon(), smsRecipient));

	SmsFutureWatcher->deleteLater();
	SmsFutureWatcher = 0;
}

void SmsHistoryTab::futureSmsCanceled()
{
	hideTabWaitOverlay();

	if (!SmsFutureWatcher)
		return;

	SmsFutureWatcher->deleteLater();
	SmsFutureWatcher = 0;
}

void SmsHistoryTab::updateData()
{
	if (SmsFutureWatcher)
		delete SmsFutureWatcher;

	QFuture<QList<QString> > futureSms = History::instance()->smsRecipientsList();
	SmsFutureWatcher = new QFutureWatcher<QList<QString> >();
	connect(SmsFutureWatcher, SIGNAL(finished()), this, SLOT(futureSmsAvailable()));
	connect(SmsFutureWatcher, SIGNAL(canceled()), this, SLOT(futureSmsCanceled()));

	SmsFutureWatcher->setFuture(futureSms);

	showTabWaitOverlay();
}
