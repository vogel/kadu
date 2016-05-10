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
#include "core/session-service.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "os/generic/window-geometry-manager.h"
#include "activate.h"

#include "gui/widgets/chat-history-tab.h"
#include "gui/widgets/search-tab.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "history-injected-factory.h"
#include "history.h"

#include "history-window.h"

HistoryWindow::HistoryWindow(QWidget *parent) :
		QWidget(parent),
		CurrentTab(-1)
{
}

HistoryWindow::~HistoryWindow()
{
}

void HistoryWindow::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void HistoryWindow::setHistory(History *history)
{
	m_history = history;
}

void HistoryWindow::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void HistoryWindow::setHistoryInjectedFactory(HistoryInjectedFactory *historyInjectedFactory)
{
	m_historyInjectedFactory = historyInjectedFactory;
}

void HistoryWindow::setSessionService(SessionService *sessionService)
{
	m_sessionService = sessionService;
}

void HistoryWindow::init()
{
	setWindowRole("kadu-history");
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowTitle(tr("History"));
	setWindowIcon(m_iconsManager->iconByPath(KaduIcon("kadu_icons/history")));

	createGui();

	new WindowGeometryManager(new ConfigFileVariantWrapper(m_configuration, "History", "HistoryWindowGeometry"), QRect(200, 200, 750, 500), this);

	connect(m_history, SIGNAL(storageChanged(HistoryStorage*)), this, SLOT(storageChanged(HistoryStorage*)));
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

	ChatTab = m_historyInjectedFactory->makeInjected<ChatHistoryTab>(TabWidget);

	StatusTab = m_historyInjectedFactory->makeInjected<HistoryMessagesTab>(TabWidget);
	StatusTab->timelineView()->setTalkableVisible(false);
	StatusTab->setClearHistoryMenuItemTitle(tr("&Clear Status History"));

	SmsTab = m_historyInjectedFactory->makeInjected<HistoryMessagesTab>(TabWidget);
	SmsTab->timelineView()->setTalkableVisible(false);
	SmsTab->setClearHistoryMenuItemTitle(tr("&Clear SMS History"));

	MySearchTab = m_historyInjectedFactory->makeInjected<SearchTab>(TabWidget);

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
	if (m_sessionService->isClosing())
		return;

	if (historyStorage)
	{
		ChatTab->setHistoryMessagesStorage(historyStorage->chatStorage());
		StatusTab->setHistoryMessagesStorage(historyStorage->statusStorage());
		SmsTab->setHistoryMessagesStorage(historyStorage->smsStorage());
		MySearchTab->setHistoryChatStorage(historyStorage->chatStorage());
		MySearchTab->setStatusStorage(historyStorage->statusStorage());
		MySearchTab->setSmsStorage(historyStorage->smsStorage());
	}
	else
	{
		ChatTab->setHistoryMessagesStorage(0);
		StatusTab->setHistoryMessagesStorage(0);
		SmsTab->setHistoryMessagesStorage(0);
		MySearchTab->setHistoryChatStorage(0);
		MySearchTab->setStatusStorage(0);
		MySearchTab->setSmsStorage(0);
	}
}

void HistoryWindow::updateData()
{
	storageChanged(m_history->currentStorage());
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
