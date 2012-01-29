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
#include <QtGui/QCheckBox>
#include <QtGui/QDateEdit>
#include <QtGui/QFormLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "activate.h"

#include "gui/widgets/timeline-chat-messages-view.h"
#include "storage/history-messages-storage.h"
#include "history.h"
#include "history-query.h"

#include "search-tab.h"

SearchTab::SearchTab(QWidget *parent) :
		HistoryTab(parent)
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
	queryFormLayout->setMargin(0);

	Query = new QLineEdit(queryFormWidget);
	Query->setMinimumWidth(200);
	queryFormLayout->addRow(tr("Search for:"), Query);

	QCheckBox *searchByDate = new QCheckBox(tr("Search by date"), queryFormWidget);
	searchByDate->setCheckState(Qt::Unchecked);
	queryFormLayout->addRow(0, searchByDate);

	QDateEdit *fromDate = new QDateEdit(queryFormWidget);
	fromDate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	fromDate->setCalendarPopup(true);
	fromDate->setEnabled(false);
	queryFormLayout->addRow(tr("From:"), fromDate);

	QDateEdit *toDate = new QDateEdit(queryFormWidget);
	toDate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	toDate->setCalendarPopup(true);
	toDate->setEnabled(false);
	queryFormLayout->addRow(tr("To:"), toDate);

	QCheckBox *searchInChats = new QCheckBox(tr("Search in chats"), queryFormWidget);
	searchInChats->setCheckState(Qt::Checked);
	searchInChats->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	queryFormLayout->addRow(0, searchInChats);

	QCheckBox *searchInStatuses = new QCheckBox(tr("Search in statuses"), queryFormWidget);
	searchInStatuses->setCheckState(Qt::Unchecked);
	queryFormLayout->addRow(0, searchInStatuses);

	QCheckBox *searchInSmses = new QCheckBox(tr("Search in smses"), queryFormWidget);
	searchInSmses->setCheckState(Qt::Unchecked);
	queryFormLayout->addRow(0, searchInSmses);

	QPushButton *searchButton = new QPushButton(tr("Search"), queryFormWidget);
	searchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	queryFormLayout->addRow(0, searchButton);

	connect(searchButton, SIGNAL(clicked()), this, SLOT(performSearch()));

	TimelineView = new TimelineChatMessagesView(true, Splitter);
	connect(TimelineView, SIGNAL(currentDateChanged()), this, SLOT(currentDateChanged()));
}

void SearchTab::performSearch()
{
	HistoryQuery query;
	query.setString(Query->text());

	TimelineView->setFutureResults(History::instance()->currentStorage()->chatStorage()->dates(query));
}

void SearchTab::currentDateChanged()
{
	const QModelIndex &currentIndex = TimelineView->timeline()->currentIndex();
	const Talkable talkable = currentIndex.data(TalkableRole).value<Talkable>();
	const QDate date = currentIndex.data(DateRole).value<QDate>();

	TimelineView->setFutureMessages(History::instance()->currentStorage()->messages(talkable, date));
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
