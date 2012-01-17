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
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/kadu-tree-view.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "model/dates-model-item.h"
#include "model/history-dates-model.h"
#include "search/history-search-parameters.h"
#include "history.h"

#include "sms-history-tab.h"

SmsHistoryTab::SmsHistoryTab(QWidget *parent) :
		HistoryTab(parent)
{
	MySmsDatesModel = new HistoryDatesModel(false, this);

	SmsDetailsPopupMenu = new QMenu(this);
	SmsDetailsPopupMenu->addAction(KaduIcon("kadu_icons/clear-history").icon(), tr("&Remove entries"),
	                               this, SLOT(removeSmstEntriesPerDate()));

	createGui();
}

SmsHistoryTab::~SmsHistoryTab()
{
}

void SmsHistoryTab::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

	FilteredTreeView *smsListWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, splitter);
	smsListWidget->setFilterAutoVisibility(false);

	SmsListView = new KaduTreeView(smsListWidget);
	SmsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	SmsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	SmsModel = new QStandardItemModel(SmsListView);
	QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(SmsModel);
	proxyModel->setSourceModel(SmsModel);

	connect(smsListWidget, SIGNAL(filterChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));

	SmsListView->setModel(proxyModel);

	connect(SmsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(currentSmsChanged(QModelIndex,QModelIndex)));
	connect(SmsListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showSmsPopupMenu(QPoint)));
	SmsListView->setContextMenuPolicy(Qt::CustomContextMenu);

	smsListWidget->setView(SmsListView);

	TimelineSmsesView = new TimelineChatMessagesView(splitter);

	TimelineSmsesView->timeline()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TimelineSmsesView->timeline(), SIGNAL(customContextMenuRequested(QPoint)),
	        this, SLOT(showSmsDetailsPopupMenu(QPoint)));

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	splitter->setSizes(sizes);

	layout->addWidget(splitter);
}

void SmsHistoryTab::updateData()
{
	QList<QString> smsRecipients = History::instance()->smsRecipientsList(HistorySearchParameters());

	SmsModel->clear();
	foreach (const QString &smsRecipient, smsRecipients)
		SmsModel->appendRow(new QStandardItem(KaduIcon("phone").icon(), smsRecipient));
}

void SmsHistoryTab::smsRecipientActivated(const QString& recipient)
{
	QModelIndex selectedIndex = TimelineSmsesView->timeline()->model()
	        ? TimelineSmsesView->timeline()->selectionModel()->currentIndex()
	        : QModelIndex();

	QDate date = selectedIndex.data(DateRole).toDate();

	QVector<DatesModelItem> smsDates = History::instance()->datesForSmsRecipient(recipient, HistorySearchParameters());
	MySmsDatesModel->setDates(smsDates);

	if (date.isValid())
		selectedIndex = MySmsDatesModel->indexForDate(date);
	if (!selectedIndex.isValid())
	{
		int lastRow = MySmsDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			selectedIndex = MySmsDatesModel->index(lastRow);
	}

	TimelineSmsesView->timeline()->setModel(MySmsDatesModel);

	connect(TimelineSmsesView->timeline()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(smsDateCurrentChanged(QModelIndex,QModelIndex)), Qt::UniqueConnection);

	TimelineSmsesView->timeline()->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void SmsHistoryTab::smsDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (current == previous)
		return;

	QDate date = current.data(DateRole).value<QDate>();

	TimelineSmsesView->messagesView()->setUpdatesEnabled(false);

	QString recipient = SmsListView->currentIndex().data().toString();
	QVector<Message> sms;
	if (!recipient.isEmpty() && date.isValid())
		sms = History::instance()->sms(recipient, date);
	TimelineSmsesView->messagesView()->setChat(Chat::null);
	TimelineSmsesView->messagesView()->clearMessages();
	TimelineSmsesView->messagesView()->appendMessages(sms);

	TimelineSmsesView->messagesView()->setUpdatesEnabled(true);
}

void SmsHistoryTab::showSmsPopupMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	QScopedPointer<QMenu> menu;

	menu.reset(new QMenu(this));
	menu->addSeparator();
	menu->addAction(KaduIcon("kadu_icons/clear-history").icon(),
			tr("&Clear SMS History"), this, SLOT(clearSmsHistory()));

	menu->exec(QCursor::pos());
}

void SmsHistoryTab::showSmsDetailsPopupMenu(const QPoint &pos)
{
	QDate date = TimelineSmsesView->timeline()->indexAt(pos).data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	if (!SmsListView->currentIndex().data().toString().isEmpty())
		SmsDetailsPopupMenu->exec(QCursor::pos());
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
		updateData();
}

void SmsHistoryTab::removeSmsEntriesPerDate()
{
	QDate date = TimelineSmsesView->timeline()->currentIndex().data(DateRole).value<QDate>();
	if (!date.isValid())
		return;

	if (!SmsListView->currentIndex().data().toString().isEmpty())
	{
		History::instance()->currentStorage()->clearSmsHistory(SmsListView->currentIndex().data().toString(), date);
		smsRecipientActivated(SmsListView->currentIndex().data().toString());
	}
}

void SmsHistoryTab::currentSmsChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous);

	smsRecipientActivated(current.data().toString());
}

void SmsHistoryTab::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == QKeySequence::Copy && !TimelineSmsesView->messagesView()->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		TimelineSmsesView->messagesView()->pageAction(QWebPage::Copy)->trigger();
	else
		HistoryTab::keyPressEvent(event);
}
