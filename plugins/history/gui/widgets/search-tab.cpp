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

#include <QtCore/QVariant>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDateEdit>
#include <QtGui/QFormLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "gui/web-view-highlighter.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/search-bar.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "activate.h"

#include "gui/widgets/history-talkable-combo-box.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "storage/history-messages-storage.h"
#include "history-query.h"

#include "search-tab.h"

SearchTab::SearchTab(QWidget *parent) :
		HistoryTab(parent), ChatStorage(0), StatusStorage(0), SmsStorage(0), SearchedStorage(&ChatStorage)
{
	createGui();
}

SearchTab::~SearchTab()
{
}

void SearchTab::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(2);
	layout->setSpacing(0);

	Splitter = new QSplitter(Qt::Horizontal, this);
	layout->addWidget(Splitter);

	QWidget *queryWidget = new QWidget(Splitter);
	QVBoxLayout *queryLayout = new QVBoxLayout(queryWidget);
	queryLayout->setMargin(3);

	QWidget *queryFormWidget = new QWidget(queryWidget);
	queryLayout->addWidget(queryFormWidget);

	QFormLayout *queryFormLayout = new QFormLayout(queryFormWidget);
	queryFormLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignHCenter);
	queryFormLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
	queryFormLayout->setMargin(0);

	Query = new QLineEdit(queryFormWidget);
	Query->setMinimumWidth(200);
	queryFormLayout->addRow(tr("Search for:"), Query);

	SearchInChats = new QRadioButton(tr("Chats"), queryFormWidget);
	SearchInChats->setChecked(true);
	SelectChat = new HistoryTalkableComboBox(queryFormWidget);
	SelectChat->setAllLabel(tr(" - All chats - "));
	SelectChat->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	queryFormLayout->addRow(SearchInChats, SelectChat);

	SearchInStatuses = new QRadioButton(tr("Statuses"), queryFormWidget);
	SelectStatusBuddy = new HistoryTalkableComboBox(queryFormWidget);
	SelectStatusBuddy->setAllLabel(tr(" - All buddies - "));
	SelectStatusBuddy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	SelectStatusBuddy->setEnabled(false);
	queryFormLayout->addRow(SearchInStatuses, SelectStatusBuddy);

	SearchInSmses = new QRadioButton(tr("Smses"), queryFormWidget);
	SelectSmsRecipient = new HistoryTalkableComboBox(queryFormWidget);
	SelectSmsRecipient->setAllLabel(tr(" - All recipients - "));
	SelectSmsRecipient->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	SelectSmsRecipient->setEnabled(false);
	queryFormLayout->addRow(SearchInSmses, SelectSmsRecipient);

	QButtonGroup *kindRadioGroup = new QButtonGroup(queryFormWidget);
	kindRadioGroup->addButton(SearchInChats);
	kindRadioGroup->addButton(SearchInStatuses);
	kindRadioGroup->addButton(SearchInSmses);
	connect(kindRadioGroup, SIGNAL(buttonReleased(QAbstractButton*)),
	        this, SLOT(kindChanged(QAbstractButton*)));

	SearchByDate = new QCheckBox(tr("By date"), queryFormWidget);
	SearchByDate->setCheckState(Qt::Unchecked);

	QWidget *dateWidget = new QWidget(queryFormWidget);

	QHBoxLayout *dateLayout = new QHBoxLayout(dateWidget);

	FromDate = new QDateEdit(dateWidget);
	FromDate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	FromDate->setCalendarPopup(true);
	FromDate->setDate(QDate::currentDate().addDays(-7));
	dateLayout->addWidget(FromDate);

	dateLayout->addWidget(new QLabel(tr("to"), dateWidget));

	ToDate = new QDateEdit(dateWidget);
	ToDate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ToDate->setCalendarPopup(true);
	ToDate->setDate(QDate::currentDate());
	dateLayout->addWidget(ToDate);

	connect(FromDate, SIGNAL(dateChanged(QDate)), this, SLOT(fromDateChanged(QDate)));
	connect(ToDate, SIGNAL(dateChanged(QDate)), this, SLOT(toDateChanged(QDate)));
	connect(SearchByDate, SIGNAL(toggled(bool)), dateWidget, SLOT(setEnabled(bool)));

	dateWidget->setEnabled(false);
	queryFormLayout->addRow(SearchByDate, dateWidget);

	QPushButton *searchButton = new QPushButton(tr("Search"), queryFormWidget);
	searchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	queryFormLayout->addRow(0, searchButton);

	connect(searchButton, SIGNAL(clicked()), this, SLOT(performSearch()));

	TimelineView = new TimelineChatMessagesView(Splitter);
	TimelineView->setTalkableVisible(true);
	TimelineView->setTitleVisible(true);
	TimelineView->setLengthHeader(tr("Found"));
	connect(TimelineView, SIGNAL(currentDateChanged()), this, SLOT(currentDateChanged()));
	connect(TimelineView, SIGNAL(messagesDisplayed()), this, SLOT(messagesDisplayed()));

	TimelineView->searchBar()->show();
	connect(TimelineView->searchBar(), SIGNAL(clearSearch()), this, SLOT(clearSelect()));

	setFocusProxy(Query);
}

void SearchTab::setChatStorage(HistoryMessagesStorage *storage)
{
	if (ChatStorage == storage)
		return;

	ChatStorage = storage;

	if (!ChatStorage)
		SelectChat->setTalkables(QVector<Talkable>());
	else
		SelectChat->setFutureTalkables(ChatStorage->talkables());

	if (*SearchedStorage == ChatStorage)
	{
		TimelineView->setResults(QVector<HistoryQueryResult>());
		TimelineView->messagesView()->setChat(Chat::null);
		TimelineView->messagesView()->clearMessages();
		TimelineView->messagesView()->refresh();
	}
}

void SearchTab::setStatusStorage(HistoryMessagesStorage *storage)
{
	if (StatusStorage == storage)
		return;

	StatusStorage = storage;

	if (!StatusStorage)
		SelectStatusBuddy->setTalkables(QVector<Talkable>());
	else
		SelectStatusBuddy->setFutureTalkables(StatusStorage->talkables());

	if (*SearchedStorage == StatusStorage)
	{
		TimelineView->setResults(QVector<HistoryQueryResult>());
		TimelineView->messagesView()->setChat(Chat::null);
		TimelineView->messagesView()->clearMessages();
		TimelineView->messagesView()->refresh();
	}
}

void SearchTab::setSmsStorage(HistoryMessagesStorage *storage)
{
	if (SmsStorage == storage)
		return;

	SmsStorage = storage;

	if (!SmsStorage)
		SelectSmsRecipient->setTalkables(QVector<Talkable>());
	else
		SelectSmsRecipient->setFutureTalkables(SmsStorage->talkables());

	if (*SearchedStorage == SmsStorage)
	{
		TimelineView->setResults(QVector<HistoryQueryResult>());
		TimelineView->messagesView()->setChat(Chat::null);
		TimelineView->messagesView()->clearMessages();
		TimelineView->messagesView()->refresh();
	}
}

void SearchTab::kindChanged(QAbstractButton *button)
{
	SelectChat->setEnabled(SearchInChats == button);
	SelectStatusBuddy->setEnabled(SearchInStatuses == button);
	SelectSmsRecipient->setEnabled(SearchInSmses == button);
}

void SearchTab::fromDateChanged(const QDate &date)
{
	if (ToDate->date() < date)
		ToDate->setDate(date);
}

void SearchTab::toDateChanged(const QDate &date)
{
	if (FromDate->date() > date)
		FromDate->setDate(date);
}

void SearchTab::performSearch()
{
	HistoryQuery query;
	query.setString(Query->text());

	if (SearchByDate->isChecked())
	{
		query.setFromDate(FromDate->date());
		query.setToDate(ToDate->date());
	}

	if (SearchInChats->isChecked())
	{
		query.setTalkable(SelectChat->currentTalkable());
		SearchedStorage = &ChatStorage;
		TimelineView->setTalkableHeader(tr("Chat"));
	}
	else if (SearchInStatuses->isChecked())
	{
		query.setTalkable(SelectStatusBuddy->currentTalkable());
		SearchedStorage = &StatusStorage;
		TimelineView->setTalkableHeader(tr("Buddy"));
	}
	else if (SearchInSmses->isChecked())
	{
		query.setTalkable(SelectSmsRecipient->currentTalkable());
		SearchedStorage = &SmsStorage;
		TimelineView->setTalkableHeader(tr("Recipient"));
	}

	if (SearchedStorage && *SearchedStorage)
		TimelineView->setFutureResults((*SearchedStorage)->dates(query));
	else
		TimelineView->setResults(QVector<HistoryQueryResult>());
}

void SearchTab::clearSelect()
{
	TimelineView->highlighter()->setHighlight(Query->text());
	TimelineView->highlighter()->selectNext(Query->text());
}

void SearchTab::currentDateChanged()
{
	const QModelIndex &currentIndex = TimelineView->timeline()->currentIndex();
	if (!currentIndex.isValid())
	{
		TimelineView->messagesView()->setChat(Chat::null);
		TimelineView->messagesView()->clearMessages();
		return;
	}

	const Talkable talkable = currentIndex.data(TalkableRole).value<Talkable>();
	const QDate date = currentIndex.data(DateRole).value<QDate>();

	Chat chat = talkable.toChat();
	if (!chat)
	{
		chat = Chat::create();
		chat.setDisplay("?");
	}
	TimelineView->messagesView()->setChat(chat);

	if (SearchedStorage && *SearchedStorage)
	{
		HistoryQuery query;
		query.setTalkable(talkable);
		query.setFromDate(date);
		query.setToDate(date);

		TimelineView->setFutureMessages((*SearchedStorage)->messages(query));
	}
	else
		TimelineView->setMessages(QVector<Message>());
}

void SearchTab::messagesDisplayed()
{
	TimelineView->searchBar()->show();
	TimelineView->searchBar()->setSearchText(Query->text());
	TimelineView->highlighter()->setHighlight(Query->text());
	TimelineView->highlighter()->selectNext(Query->text());
}

QList<int> SearchTab::sizes() const
{
	QList<int> result = Splitter->sizes();
	result.append(TimelineView->sizes());

	return result;
}

void SearchTab::setSizes(const QList<int> &newSizes)
{
	Q_ASSERT(newSizes.size() == 4);

	Splitter->setSizes(newSizes.mid(0, 2));
	TimelineView->setSizes(newSizes.mid(2, 2));
}
