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

#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "chat/model/chats-list-model.h"
#include "buddies/model/buddy-list-model.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "gui/widgets/wait-overlay.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "icons/kadu-icon.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "model/dates-model-item.h"
#include "model/history-dates-model.h"
#include "storage/history-messages-storage.h"
#include "chats-buddies-splitter.h"

#include "history-tab.h"

HistoryTab::HistoryTab(bool showTitleInTimeline, QWidget *parent) :
		QWidget(parent), Storage(0),
		TabWaitOverlay(0), TimelineWaitOverlay(0), MessagesViewWaitOverlay(0),
		TalkablesFutureWatcher(0), DatesFutureWatcher(0), MessagesFutureWatcher(0)
{
	DatesModel = new HistoryDatesModel(showTitleInTimeline, this);

	createGui();
	createModelChain();
}

HistoryTab::~HistoryTab()
{
}

void HistoryTab::createGui()
{
	TimelinePopupMenu = new QMenu(this);
	TimelinePopupMenu->addAction(KaduIcon("kadu_icons/clear-history").icon(), tr("&Remove entries"),
	                             this, SLOT(removeEntries()));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(2);

	Splitter = new QSplitter(Qt::Horizontal, this);

	FilteredView = new FilteredTreeView(FilteredTreeView::FilterAtTop, Splitter);
	FilteredView->filterWidget()->setAutoVisibility(false);
	FilteredView->filterWidget()->setLabel(tr("Filter") + ":");

	TalkableTree = new TalkableTreeView(FilteredView);
	TalkableTree->setAlternatingRowColors(true);
	TalkableTree->setContextMenuEnabled(true);
	TalkableTree->setContextMenuPolicy(Qt::CustomContextMenu);
	TalkableTree->setUseConfigurationColors(true);
	TalkableTree->delegateConfiguration().setShowMessagePixmap(false);

	QString style;
	style.append("QTreeView::branch:has-siblings:!adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-siblings:adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-childres:!has-siblings:adjoins-item { border-image: none; image: none }");
	TalkableTree->setStyleSheet(style);
	TalkableTree->viewport()->setStyleSheet(style);

	connect(TalkableTree, SIGNAL(currentChanged(Talkable)), this, SLOT(currentTalkableChanged(Talkable)));
	connect(TalkableTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showTalkablePopupMenu()));

	FilteredView->setView(TalkableTree);

	TimelineView = new TimelineChatMessagesView(Splitter);
	TimelineView->timeline()->setModel(DatesModel);
	TimelineView->timeline()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TimelineView->timeline(), SIGNAL(customContextMenuRequested(QPoint)),
	        this, SLOT(showTimelinePopupMenu()));
	connect(TimelineView->timeline()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SLOT(currentDateChanged()));

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	Splitter->setSizes(sizes);

	layout->addWidget(Splitter);
}

void HistoryTab::createModelChain()
{
	ChatsModel = new ChatsListModel(TalkableTree);
	BuddiesModel = new BuddyListModel(TalkableTree);

	QList<QAbstractItemModel *> models;
	models.append(ChatsModel);
	models.append(BuddiesModel);

	QAbstractItemModel *mergedModel = MergedProxyModelFactory::createKaduModelInstance(models, TalkableTree);

	Chain = new ModelChain(mergedModel, TalkableTree);

	TalkableProxyModel *proxyModel = new TalkableProxyModel(Chain);
	proxyModel->setSortByStatusAndUnreadMessages(false);

	NameTalkableFilter *nameTalkableFilter = new NameTalkableFilter(NameTalkableFilter::AcceptMatching, proxyModel);
	connect(FilteredView, SIGNAL(filterChanged(QString)), nameTalkableFilter, SLOT(setName(QString)));
	proxyModel->addFilter(nameTalkableFilter);

	Chain->addProxyModel(proxyModel);

	TalkableTree->setChain(Chain);
}

void HistoryTab::displayTalkable(const Talkable &talkable, bool force)
{
	if (!force && CurrentTalkable == talkable)
		return;

	CurrentTalkable = talkable;
	TimelineView->messagesView()->setChat(CurrentTalkable.toChat());

	if (Storage)
		setFutureDates(Storage->dates(CurrentTalkable));
	else
		setDates(QVector<DatesModelItem>());
}

FilteredTreeView * HistoryTab::filteredView() const
{
	return FilteredView;
}

TalkableTreeView * HistoryTab::talkableTree() const
{
	return TalkableTree;
}

ModelChain * HistoryTab::modelChain() const
{
	return Chain;
}

void HistoryTab::showTabWaitOverlay()
{
	if (!TabWaitOverlay)
		TabWaitOverlay = new WaitOverlay(this);
	else
		TabWaitOverlay->show();
}

void HistoryTab::hideTabWaitOverlay()
{
	TabWaitOverlay->deleteLater();
	TabWaitOverlay = 0;
}

void HistoryTab::showTimelineWaitOverlay()
{
	if (!TimelineWaitOverlay)
		TimelineWaitOverlay = new WaitOverlay(TimelineView);
	else
		TimelineWaitOverlay->show();
}

void HistoryTab::hideTimelineWaitOverlay()
{
	TimelineWaitOverlay->deleteLater();
	TimelineWaitOverlay = 0;
}

void HistoryTab::showMessagesViewWaitOverlay()
{
	if (!MessagesViewWaitOverlay)
		MessagesViewWaitOverlay = new WaitOverlay(TimelineView->messagesView());
	else
		MessagesViewWaitOverlay->show();
}

void HistoryTab::hideMessagesViewWaitOverlay()
{
	MessagesViewWaitOverlay->deleteLater();
	MessagesViewWaitOverlay = 0;
}

void HistoryTab::talkablesAvailable()
{
}

TimelineChatMessagesView * HistoryTab::timelineView() const
{
	return TimelineView;
}

void HistoryTab::setTalkables(const QVector<Talkable> &talkables)
{
	ChatsBuddiesSplitter chatsBuddies(talkables);

	ChatsModel->setChats(chatsBuddies.chats().toList().toVector());
	BuddiesModel->setBuddyList(chatsBuddies.buddies().toList());
}

void HistoryTab::futureTalkablesAvailable()
{
	hideTabWaitOverlay();

	if (!TalkablesFutureWatcher)
		return;

	setTalkables(TalkablesFutureWatcher->result());

	TalkablesFutureWatcher->deleteLater();
	TalkablesFutureWatcher = 0;

	talkablesAvailable();
}

void HistoryTab::futureTalkablesCanceled()
{
	hideTabWaitOverlay();

	if (!TalkablesFutureWatcher)
		return;

	TalkablesFutureWatcher->deleteLater();
	TalkablesFutureWatcher = 0;
}

void HistoryTab::setFutureTalkables(const QFuture<QVector<Talkable> > &futureTalkables)
{
	setTalkables(QVector<Talkable>());

	if (TalkablesFutureWatcher)
	{
		TalkablesFutureWatcher->cancel();
		TalkablesFutureWatcher->deleteLater();
	}

	TalkablesFutureWatcher = new QFutureWatcher<QVector<Talkable> >(this);
	connect(TalkablesFutureWatcher, SIGNAL(finished()), this, SLOT(futureTalkablesAvailable()));
	connect(TalkablesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureTalkablesCanceled()));

	TalkablesFutureWatcher->setFuture(futureTalkables);

	showTabWaitOverlay();
}

void HistoryTab::setDates(const QVector<DatesModelItem> &dates)
{
	DatesModel->setDates(dates);

	if (!dates.isEmpty())
	{
		TimelineView->timeline()->setCurrentIndex(TimelineView->timeline()->model()->index(dates.size() - 1, 0));
		QScrollBar *scrollBar = TimelineView->timeline()->verticalScrollBar();
		scrollBar->setValue(scrollBar->maximum());
	}
	else
		currentDateChanged();
}

void HistoryTab::futureDatesAvailable()
{
	hideTimelineWaitOverlay();

	if (!DatesFutureWatcher)
		return;

	setDates(DatesFutureWatcher->result());

	DatesFutureWatcher->deleteLater();
	DatesFutureWatcher = 0;
}

void HistoryTab::futureDatesCanceled()
{
	hideTimelineWaitOverlay();

	if (!DatesFutureWatcher)
		return;

	DatesFutureWatcher->deleteLater();
	DatesFutureWatcher = 0;
}

void HistoryTab::setFutureDates(const QFuture<QVector<DatesModelItem> > &futureDates)
{
	setDates(QVector<DatesModelItem>());

	if (DatesFutureWatcher)
	{
		DatesFutureWatcher->cancel();
		DatesFutureWatcher->deleteLater();
	}

	DatesFutureWatcher = new QFutureWatcher<QVector<DatesModelItem> >(this);
	connect(DatesFutureWatcher, SIGNAL(finished()), this, SLOT(futureDatesAvailable()));
	connect(DatesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureDatesCanceled()));

	DatesFutureWatcher->setFuture(futureDates);

	showTimelineWaitOverlay();
}

void HistoryTab::setMessages(const QVector<Message> &messages)
{
	timelineView()->messagesView()->setUpdatesEnabled(false);

	timelineView()->messagesView()->clearMessages();
	timelineView()->messagesView()->appendMessages(messages);
	timelineView()->messagesView()->refresh();

	timelineView()->messagesView()->setUpdatesEnabled(true);
}

void HistoryTab::futureMessagesAvailable()
{
	if (!MessagesFutureWatcher)
	{
		hideMessagesViewWaitOverlay();
		return;
	}

	setMessages(MessagesFutureWatcher->result());
	hideMessagesViewWaitOverlay(); // wait for messages to display before hiding

	MessagesFutureWatcher->deleteLater();
	MessagesFutureWatcher = 0;
}

void HistoryTab::futureMessagesCanceled()
{
	hideMessagesViewWaitOverlay();

	if (!MessagesFutureWatcher)
		return;

	MessagesFutureWatcher->deleteLater();
	MessagesFutureWatcher = 0;
}

void HistoryTab::setFutureMessages(const QFuture<QVector<Message> > &futureMessages)
{
	setMessages(QVector<Message>());

	if (MessagesFutureWatcher)
	{
		MessagesFutureWatcher->cancel();
		MessagesFutureWatcher->deleteLater();
	}

	MessagesFutureWatcher = new QFutureWatcher<QVector<Message> >(this);
	connect(MessagesFutureWatcher, SIGNAL(finished()), this, SLOT(futureMessagesAvailable()));
	connect(MessagesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureMessagesCanceled()));

	MessagesFutureWatcher->setFuture(futureMessages);

	showMessagesViewWaitOverlay();
}

void HistoryTab::currentTalkableChanged(const Talkable &talkable)
{
	displayTalkable(talkable, false);
}

void HistoryTab::currentDateChanged()
{
	QDate date = timelineView()->currentDate();

	if (!Storage || !date.isValid())
	{
		setMessages(QVector<Message>());
		return;
	}

	timelineView()->messagesView()->setChat(CurrentTalkable.toChat());
	setFutureMessages(Storage->messages(CurrentTalkable, date));
}

void HistoryTab::showTalkablePopupMenu()
{
	QScopedPointer<QMenu> menu;
	menu.reset(TalkableMenuManager::instance()->menu(this, TalkableTree->actionContext()));
	modifyTalkablePopupMenu(menu);

	menu->exec(QCursor::pos());
}

void HistoryTab::showTimelinePopupMenu()
{
	if (TimelineView->currentDate().isValid())
		TimelinePopupMenu->exec(QCursor::pos());
}

void HistoryTab::removeEntries()
{
	QDate date = TimelineView->currentDate();
	if (!Storage || !date.isValid())
		return;

	Storage->deleteMessages(CurrentTalkable, date);
	displayTalkable(CurrentTalkable, true);
}

void HistoryTab::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == QKeySequence::Copy && !TimelineView->messagesView()->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		TimelineView->messagesView()->pageAction(QWebPage::Copy)->trigger();
	else
		QWidget::keyPressEvent(event);
}

void HistoryTab::updateData()
{
	if (!Storage)
	{
		setTalkables(QVector<Talkable>());
		displayTalkable(Talkable(), false);
		return;
	}

	setFutureTalkables(Storage->talkables());
}

QList<int> HistoryTab::sizes() const
{
	QList<int> result = Splitter->sizes();
	result.append(TimelineView->sizes());

	return result;
}

void HistoryTab::setSizes(const QList<int> &newSizes)
{
	Q_ASSERT(newSizes.size() == 4);

	Splitter->setSizes(newSizes.mid(0, 2));
	TimelineView->setSizes(newSizes.mid(2, 2));
}

void HistoryTab::setHistoryMessagesStorage(HistoryMessagesStorage *storage)
{
	Storage = storage;
	updateData();
}

HistoryMessagesStorage * HistoryTab::historyMessagesStorage() const
{
	return Storage;
}
