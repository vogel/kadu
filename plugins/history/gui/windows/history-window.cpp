/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QScopedPointer>
#include <QtGui/QDateEdit>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QItemDelegate>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/model/buddies-model-base.h"
#include "buddies/model/buddy-list-model.h"
#include "chat/aggregate-chat-manager.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "chat/model/chats-list-model.h"
#include "chat/type/chat-type-manager.h"
#include "chat/type/chat-type.h"
#include "gui/actions/actions.h"
#include "gui/actions/base-action-context.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/delayed-line-edit.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "message/message.h"
#include "misc/misc.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "activate.h"
#include "debug.h"

#include "model/dates-model-item.h"
#include "model/history-dates-model.h"
#include "gui/widgets/chat-history-tab.h"
#include "gui/widgets/sms-history-tab.h"
#include "gui/widgets/status-history-tab.h"
#include "gui/widgets/timeline-chat-messages-view.h"
#include "search/history-search-parameters.h"
#include "storage/history-storage.h"
#include "history.h"
#include "timed-status.h"

#include "history-window.h"

HistoryWindow * HistoryWindow::Instance = 0;

void HistoryWindow::show(const Chat &chat)
{
	if (!History::instance()->currentStorage())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("There is no history storage plugin loaded!"));
		return;
	}

	Chat aggregate = AggregateChatManager::instance()->aggregateChat(chat);
	if (!aggregate)
		aggregate = chat;

	if (!Instance)
		Instance = new HistoryWindow();

	Instance->updateData();
	Instance->selectChat(aggregate);

	Instance->setVisible(true);
	_activateWindow(Instance);
}

HistoryWindow::HistoryWindow(QWidget *parent) :
		QMainWindow(parent)
{
	kdebugf();

	setProperty("ownWindowIcon", true);
	setWindowRole("kadu-history");
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowTitle(tr("History"));
	setWindowIcon(KaduIcon("kadu_icons/history").icon());

	createGui();

	loadWindowGeometry(this, "History", "HistoryWindowGeometry", 200, 200, 750, 500);

	kdebugf2();
}

HistoryWindow::~HistoryWindow()
{
	kdebugf();

	saveWindowGeometry(this, "History", "HistoryDialogGeometry");

	Instance = 0;

	kdebugf2();
}

void HistoryWindow::createGui()
{
	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(mainWidget);
	layout->setMargin(0);
	layout->setSpacing(0);

	TabWidget = new QTabWidget(mainWidget);
	TabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	TabWidget->setDocumentMode(true);

	ChatTab = new ChatHistoryTab(TabWidget);
	StatusTab = new StatusHistoryTab(TabWidget);
	SmsTab = new SmsHistoryTab(TabWidget);

	TabWidget->addTab(ChatTab, tr("Chats"));
	TabWidget->addTab(StatusTab, tr("Statuses"));
	TabWidget->addTab(SmsTab, tr("SMS"));

	QDialogButtonBox *buttons = new QDialogButtonBox(mainWidget);
	buttons->addButton(tr("Search in History..."), QDialogButtonBox::ActionRole);
	QPushButton *closeButton = buttons->addButton(QDialogButtonBox::Close);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	buttons->layout()->setMargin(5);

	layout->addWidget(TabWidget);
	layout->addWidget(buttons);

	setCentralWidget(mainWidget);
}

void HistoryWindow::updateData()
{
	kdebugf();

	ChatTab->updateData();
	StatusTab->updateData();
	SmsTab->updateData();

	selectChat(Chat::null);
}

void HistoryWindow::selectChat(const Chat &chat)
{
	TabWidget->setCurrentIndex(0);
	ChatTab->selectChat(chat);
}

void HistoryWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
