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
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
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
	queryFormLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
	queryFormLayout->setMargin(0);

	Query = new QLineEdit(queryFormWidget);
	Query->setMinimumWidth(200);
	queryFormLayout->addRow(tr("Search for:"), Query);

	QRadioButton *searchInChats = new QRadioButton(tr("Search in chats"), queryFormWidget);
	searchInChats->setChecked(true);
	searchInChats->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	queryFormLayout->addRow(0, searchInChats);

	QRadioButton *searchInStatuses = new QRadioButton(tr("Search in statuses"), queryFormWidget);
	queryFormLayout->addRow(0, searchInStatuses);

	QRadioButton *searchInSmses = new QRadioButton(tr("Search in smses"), queryFormWidget);
	queryFormLayout->addRow(0, searchInSmses);

	QButtonGroup *kindRadioGroup = new QButtonGroup(queryFormWidget);
	kindRadioGroup->addButton(searchInChats);
	kindRadioGroup->addButton(searchInStatuses);
	kindRadioGroup->addButton(searchInSmses);

	SearchByDate = new QCheckBox(tr("Search by date"), queryFormWidget);
	SearchByDate->setCheckState(Qt::Unchecked);
	queryFormLayout->addRow(0, SearchByDate);

	QWidget *dateWidget = new QWidget(queryFormWidget);
	dateWidget->hide();

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
	connect(SearchByDate, SIGNAL(toggled(bool)), dateWidget, SLOT(setVisible(bool)));

	queryFormLayout->addRow(dateWidget);

	QPushButton *searchButton = new QPushButton(tr("Search"), queryFormWidget);
	searchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	queryFormLayout->addRow(0, searchButton);

	connect(searchButton, SIGNAL(clicked()), this, SLOT(performSearch()));

	TimelineView = new TimelineChatMessagesView(true, Splitter);
	connect(TimelineView, SIGNAL(currentDateChanged()), this, SLOT(currentDateChanged()));
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
