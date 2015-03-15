/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include "chat-style/engine/chat-style-renderer-factory-provider.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "formatted-string/formatted-string-plain-text-visitor.h"
#include "formatted-string/formatted-string.h"
#include "gui/scoped-updates-disabler.h"
#include "gui/web-view-highlighter.h"
#include "gui/widgets/search-bar.h"
#include "gui/widgets/wait-overlay.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"
#include "message/message-manager.h"
#include "message/sorted-messages.h"
#include "model/roles.h"

#include "model/history-query-results-model.h"
#include "model/history-query-results-proxy-model.h"
#include "history-query-result.h"

#include "timeline-chat-messages-view.h"

#define DATE_TITLE_LENGTH 120

TimelineChatMessagesView::TimelineChatMessagesView(QWidget *parent) :
		QWidget(parent),
		TimelineWaitOverlay(0), MessagesViewWaitOverlay(0),
		ResultsFutureWatcher (0), MessagesFutureWatcher(0)
{
	ResultsModel = new HistoryQueryResultsModel(this);
	ResultsProxyModel = new HistoryQueryResultsProxyModel(ResultsModel);
	ResultsProxyModel->setSourceModel(ResultsModel);

	setLayout(new QVBoxLayout(this));
	layout()->setMargin(0);
	layout()->setSpacing(0);

	createGui();

	connect(MessageManager::instance(), SIGNAL(messageReceived(Message)), this, SLOT(newMessage(Message)));
	connect(MessageManager::instance(), SIGNAL(messageSent(Message)), this, SLOT(newMessage(Message)));
}

TimelineChatMessagesView::~TimelineChatMessagesView()
{
}

void TimelineChatMessagesView::createGui()
{
	Splitter = new QSplitter(Qt::Vertical, this);

	Timeline = new QTreeView(Splitter);

	Timeline->setAlternatingRowColors(true);
	Timeline->setModel(ResultsProxyModel);
	Timeline->setRootIsDecorated(false);
	Timeline->setUniformRowHeights(true);

	connect(Timeline->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
	        this, SIGNAL(currentDateChanged()));

	QFrame *frame = new QFrame(Splitter);
	frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

	QVBoxLayout *frameLayout = new QVBoxLayout(frame);
	frameLayout->setMargin(0);
	frameLayout->setSpacing(0);

	MessagesView = Core::instance()->webkitMessagesViewFactory()->createWebkitMessagesView(Chat::null, false, frame);
	MessagesView->setFocusPolicy(Qt::StrongFocus);
	MessagesView->setForcePruneDisabled(true);

	frameLayout->addWidget(MessagesView.get());

	MessagesSearchBar = new SearchBar(this);
	MessagesSearchBar->setSearchWidget(MessagesView.get());

	Highlighter = new WebViewHighlighter(MessagesView.get());
	Highlighter->setAutoUpdate(true);

	connect(MessagesSearchBar, SIGNAL(searchPrevious(QString)), Highlighter, SLOT(selectPrevious(QString)));
	connect(MessagesSearchBar, SIGNAL(searchNext(QString)), Highlighter, SLOT(selectNext(QString)));
	connect(MessagesSearchBar, SIGNAL(clearSearch()), Highlighter, SLOT(clearSelect()));
	connect(Highlighter, SIGNAL(somethingFound(bool)), MessagesSearchBar, SLOT(somethingFound(bool)));

	frameLayout->addWidget(MessagesSearchBar);

	layout()->addWidget(Splitter);
}

QDate TimelineChatMessagesView::currentDate() const
{
	return Timeline->currentIndex().data(DateRole).value<QDate>();
}

void TimelineChatMessagesView::setResults(const QVector<HistoryQueryResult> &results)
{
	ResultsModel->setResults(results);

	if (results.isEmpty())
	{
		emit currentDateChanged();
		return;
	}

	const QModelIndex &selected = Timeline->model()->index(Timeline->model()->rowCount() - 1, 0);
	Timeline->setCurrentIndex(selected);
	Timeline->scrollTo(selected, QAbstractItemView::PositionAtBottom);
}

void TimelineChatMessagesView::futureResultsAvailable()
{
	hideTimelineWaitOverlay();

	if (!ResultsFutureWatcher)
		return;

	setResults(ResultsFutureWatcher->result());

	ResultsFutureWatcher->deleteLater();
	ResultsFutureWatcher = 0;
}

void TimelineChatMessagesView::futureResultsCanceled()
{
	hideTimelineWaitOverlay();

	if (!ResultsFutureWatcher)
		return;

	ResultsFutureWatcher->deleteLater();
	ResultsFutureWatcher = 0;
}

void TimelineChatMessagesView::setFutureResults(const QFuture<QVector<HistoryQueryResult>> &futureResults)
{
	if (ResultsFutureWatcher)
	{
		ResultsFutureWatcher->cancel();
		ResultsFutureWatcher->deleteLater();
	}

	ResultsFutureWatcher = new QFutureWatcher<QVector<HistoryQueryResult> >(this);
	connect(ResultsFutureWatcher, SIGNAL(finished()), this, SLOT(futureResultsAvailable()));
	connect(ResultsFutureWatcher, SIGNAL(canceled()), this, SLOT(futureResultsCanceled()));

	ResultsFutureWatcher->setFuture(futureResults);

	showTimelineWaitOverlay();
}

void TimelineChatMessagesView::setMessages(const SortedMessages &messages)
{
	ScopedUpdatesDisabler updatesDisabler{*MessagesView};

	MessagesView->clearMessages();
	MessagesView->add(messages);

	emit messagesDisplayed();
}

void TimelineChatMessagesView::newMessage(const Message &message)
{
	auto chatMatch = message.messageChat() == MessagesView->chat();
	if (!chatMatch)
	{
		if (message.messageChat().type() != "Contact" || MessagesView->chat().type() != "Buddy")
			return;
		if (!MessagesView->chat().contacts().toBuddySet().contains(message.messageChat().contacts().toContact().ownerBuddy()))
			return;
	}

	FormattedStringPlainTextVisitor plainTextVisitor;
	message.content()->accept(&plainTextVisitor);

	auto title = plainTextVisitor.result().replace('\n', ' ').replace('\r', ' ');
	if (title.length() > DATE_TITLE_LENGTH)
	{
		title.truncate(DATE_TITLE_LENGTH);
		title += " ...";
	}

	auto messageDate = message.receiveDate().date();
	ResultsModel->addEntry(messageDate, message.messageChat(), title);

	if (messageDate == currentDate())
		MessagesView->add(message);
}

void TimelineChatMessagesView::futureMessagesAvailable()
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

void TimelineChatMessagesView::futureMessagesCanceled()
{
	hideMessagesViewWaitOverlay();

	if (!MessagesFutureWatcher)
		return;

	MessagesFutureWatcher->deleteLater();
	MessagesFutureWatcher = 0;
}

void TimelineChatMessagesView::setFutureMessages(const QFuture<SortedMessages> &futureMessages)
{
	if (MessagesFutureWatcher)
	{
		MessagesFutureWatcher->cancel();
		MessagesFutureWatcher->deleteLater();
	}

	MessagesFutureWatcher = new QFutureWatcher<SortedMessages>(this);
	connect(MessagesFutureWatcher, SIGNAL(finished()), this, SLOT(futureMessagesAvailable()));
	connect(MessagesFutureWatcher, SIGNAL(canceled()), this, SLOT(futureMessagesCanceled()));

	MessagesFutureWatcher->setFuture(futureMessages);

	showMessagesViewWaitOverlay();
}

void TimelineChatMessagesView::showTimelineWaitOverlay()
{
	if (!TimelineWaitOverlay)
		TimelineWaitOverlay = new WaitOverlay(this);
	else
		TimelineWaitOverlay->show();
}

void TimelineChatMessagesView::hideTimelineWaitOverlay()
{
	TimelineWaitOverlay->deleteLater();
	TimelineWaitOverlay = 0;
}

void TimelineChatMessagesView::showMessagesViewWaitOverlay()
{
	if (!MessagesViewWaitOverlay)
		MessagesViewWaitOverlay = new WaitOverlay(MessagesView.get());
	else
		MessagesViewWaitOverlay->show();
}

void TimelineChatMessagesView::hideMessagesViewWaitOverlay()
{
	MessagesViewWaitOverlay->deleteLater();
	MessagesViewWaitOverlay = 0;
}

void TimelineChatMessagesView::setTalkableVisible(const bool talkableVisible)
{
	ResultsProxyModel->setTalkableVisible(talkableVisible);
}

void TimelineChatMessagesView::setTitleVisible(const bool titleVisible)
{
	ResultsProxyModel->setTitleVisible(titleVisible);
}

void TimelineChatMessagesView::setTalkableHeader(const QString &talkableHeader)
{
	ResultsModel->setTalkableHeader(talkableHeader);
}

void TimelineChatMessagesView::setLengthHeader(const QString &lengthHeader)
{
	ResultsModel->setLengthHeader(lengthHeader);
}

QList<int> TimelineChatMessagesView::sizes() const
{
	return Splitter->sizes();
}

void TimelineChatMessagesView::setSizes(const QList<int> &newSizes)
{
	Q_ASSERT(newSizes.size() == 2);

	Splitter->setSizes(newSizes);
}

#include "moc_timeline-chat-messages-view.cpp"
