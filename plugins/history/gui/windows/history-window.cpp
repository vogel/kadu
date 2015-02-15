/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "chat/buddy-chat-manager.h"
#include "configuration/config-file-variant-wrapper.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "os/generic/window-geometry-manager.h"
#include "activate.h"

#include "gui/widgets/chat-history-tab.h"
#include "gui/widgets/search-tab.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "history.h"

#include "history-window.h"

HistoryWindow * HistoryWindow::Instance = 0;

HistoryWindow * HistoryWindow::instance()
{
	return Instance;
}

void HistoryWindow::show(const Chat &chat)
{
	Chat buddyChat = BuddyChatManager::instance()->buddyChat(chat);
	if (!buddyChat)
		buddyChat = chat;

	if (!Instance)
		Instance = new HistoryWindow();

	Instance->updateData();
	Instance->selectChat(buddyChat);

	Instance->setVisible(true);
	_activateWindow(Instance);
}

HistoryWindow::HistoryWindow(QWidget *parent) :
		QWidget(parent), CurrentTab(-1)
{
	setWindowRole("kadu-history");
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowTitle(tr("History"));
	setWindowIcon(KaduIcon("kadu_icons/history").icon());

	createGui();

	new WindowGeometryManager(new ConfigFileVariantWrapper("History", "HistoryWindowGeometry"), QRect(200, 200, 750, 500), this);

	connect(History::instance(), SIGNAL(storageChanged(HistoryStorage*)), this, SLOT(storageChanged(HistoryStorage*)));
}

HistoryWindow::~HistoryWindow()
{
	disconnect(History::instance(), 0, this, 0);

	Instance = 0;
}

void HistoryWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	TabWidget = new QTabWidget(this);
	TabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	TabWidget->setDocumentMode(true);

	connect(TabWidget, SIGNAL(currentChanged(int)),
			this, SLOT(currentTabChanged(int)));

	ChatTab = new ChatHistoryTab(TabWidget);

	StatusTab = new HistoryMessagesTab(TabWidget);
	StatusTab->timelineView()->setTalkableVisible(false);
	StatusTab->setClearHistoryMenuItemTitle(tr("&Clear Status History"));

	SmsTab = new HistoryMessagesTab(TabWidget);
	SmsTab->timelineView()->setTalkableVisible(false);
	SmsTab->setClearHistoryMenuItemTitle(tr("&Clear SMS History"));

	MySearchTab = new SearchTab(TabWidget);

	TabWidget->addTab(ChatTab, tr("Chats"));
	TabWidget->addTab(StatusTab, tr("Statuses"));
	TabWidget->addTab(SmsTab, tr("SMS"));
	TabWidget->addTab(MySearchTab, tr("Search"));

	CurrentTab = 0;

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	QPushButton *closeButton = buttons->addButton(QDialogButtonBox::Close);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	buttons->layout()->setMargin(5);

	layout->addWidget(TabWidget);
	layout->addWidget(buttons);

	ChatTab->setFocus();
}

void HistoryWindow::keyPressEvent(QKeyEvent *event)
{
	QWidget::keyPressEvent(event);

	if (event->key() == Qt::Key_Escape)
		close();
}

void HistoryWindow::storageChanged(HistoryStorage *historyStorage)
{
	// TODO: fix it right, this is workaround only for crash when closing kadu with this window open
	if (Core::instance() && Core::instance()->isClosing())
		return;

	if (historyStorage)
	{
		ChatTab->setHistoryMessagesStorage(historyStorage->chatStorage());
		StatusTab->setHistoryMessagesStorage(historyStorage->statusStorage());
		SmsTab->setHistoryMessagesStorage(historyStorage->smsStorage());
		MySearchTab->setChatStorage(historyStorage->chatStorage());
		MySearchTab->setStatusStorage(historyStorage->statusStorage());
		MySearchTab->setSmsStorage(historyStorage->smsStorage());
	}
	else
	{
		ChatTab->setHistoryMessagesStorage(0);
		StatusTab->setHistoryMessagesStorage(0);
		SmsTab->setHistoryMessagesStorage(0);
		MySearchTab->setChatStorage(0);
		MySearchTab->setStatusStorage(0);
		MySearchTab->setSmsStorage(0);
	}
}

void HistoryWindow::updateData()
{
	storageChanged(History::instance()->currentStorage());
}

void HistoryWindow::selectChat(const Chat &chat)
{
	TabWidget->setCurrentIndex(0);
	ChatTab->selectTalkable(chat);
}

void HistoryWindow::currentTabChanged(int newTabIndex)
{
	if (CurrentTab < 0 || CurrentTab > TabWidget->count() ||
	    newTabIndex < 0 || newTabIndex > TabWidget->count())
	{
		CurrentTab = newTabIndex;
		return;
	}

	HistoryTab *previousTab = static_cast<HistoryTab *>(TabWidget->widget(CurrentTab));
	HistoryTab *currentTab = static_cast<HistoryTab *>(TabWidget->widget(newTabIndex));

	CurrentTab = newTabIndex;

	currentTab->setSizes(previousTab->sizes());
}

#include "moc_history-window.cpp"
