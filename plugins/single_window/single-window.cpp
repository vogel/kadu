/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QStringList>
#include <QtGui/QCloseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>

#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/kadu-window.h"
#include "icons/kadu-icon.h"
#include "message/unread-message-repository.h"
#include "os/generic/window-geometry-manager.h"
#include "provider/default-provider.h"
#include "activate.h"
#include "debug.h"
#include "kadu-application.h"

#include "single-window.h"

SingleWindowManager::SingleWindowManager(QObject *parent) :
		ConfigurationUiHandler(parent),
		m_windowProvider(new SimpleProvider<QWidget *>(0))
{
	config_file.addVariable("SingleWindow", "RosterPosition", 0);
	config_file.addVariable("SingleWindow", "KaduWindowWidth", 205);

	m_window = new SingleWindow();
	m_windowProvider->provideValue(m_window);

	Core::instance()->mainWindowProvider()->installCustomProvider(m_windowProvider);
}

SingleWindowManager::~SingleWindowManager()
{
	Core::instance()->mainWindowProvider()->removeCustomProvider(m_windowProvider);

	m_windowProvider->provideValue(0);
	delete m_window;
}

void SingleWindowManager::configurationUpdated()
{
	int newRosterPos = config_file.readNumEntry("SingleWindow", "RosterPosition", 0);
	if (m_window->rosterPosition() != newRosterPos)
		m_window->changeRosterPos(newRosterPos);
}

SingleWindow::SingleWindow()
{
	setWindowRole("kadu-single-window");

	KaduWindow *kadu = Core::instance()->kaduWindow();
	bool visible = kadu->isVisible();

	m_split = new QSplitter(Qt::Horizontal, this);

	m_tabs = new QTabWidget(this);
	m_tabs->setTabsClosable(true);

	m_rosterPos = config_file.readNumEntry("SingleWindow", "RosterPosition", 0);
	if (m_rosterPos == 0)
	{
		m_split->addWidget(kadu);
		m_split->addWidget(m_tabs);
	}
	else
	{
		m_split->addWidget(m_tabs);
		m_split->addWidget(kadu);
	}

	kadu->setMaximumWidth(QWIDGETSIZE_MAX);
	m_tabs->setMaximumWidth(QWIDGETSIZE_MAX);
	kadu->setMinimumWidth(170);
	m_tabs->setMinimumWidth(200);

	new WindowGeometryManager(new ConfigFileVariantWrapper("SingleWindow", "WindowGeometry"), QRect(0, 0, 800, 440), this);

	int kaduwidth = config_file.readNumEntry("SingleWindow", "KaduWindowWidth", 205);

	if (m_rosterPos == 0)
	{
		m_splitSizes.append(kaduwidth);
		m_splitSizes.append(width() - kaduwidth);
	}
	else
	{
		m_splitSizes.append(width() - kaduwidth);
		m_splitSizes.append(kaduwidth);
	}
	m_split->setSizes(m_splitSizes);

	setWindowTitle(kadu->windowTitle());

	connect(m_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(m_tabs, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));

	connect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(onkaduKeyPressed(QKeyEvent *)));

	setFocusProxy(kadu);
	kadu->show();
	kadu->setFocus();

	setVisible(visible);
}

SingleWindow::~SingleWindow()
{
	KaduWindow *kadu = Core::instance()->kaduWindow();
	bool visible = isVisible();

	config_file.writeEntry("SingleWindow", "KaduWindowWidth", kadu->width());

	disconnect(Core::instance()->chatWidgetManager(), 0, this, 0);
	disconnect(m_tabs, 0, this, 0);
	disconnect(kadu, 0, this, 0);

	if (!Core::instance()->isClosing())
	{
		for (int i = m_tabs->count()-1; i >= 0; --i)
		{
			ChatWidget *chatWidget = static_cast<ChatWidget *>(m_tabs->widget(i));
			const Chat &chat = chatWidget->chat();
			m_tabs->removeTab(i);
			chatWidget->requestClose();
			Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::DoNotActivate);
		}
	}

	kadu->setParent(0);
	if (!Core::instance()->isClosing())
		kadu->setVisible(visible);
}

void SingleWindow::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if (event->type() == QEvent::ActivationChange)
	{
		ChatWidget *chatWidget = static_cast<ChatWidget *>(m_tabs->currentWidget());
		if (chatWidget && _isActiveWindow(this))
			emit chatWidgetActivated(chatWidget);
	}
}

void SingleWindow::changeRosterPos(int newRosterPos)
{
	m_rosterPos = newRosterPos;
	m_split->insertWidget(m_rosterPos, Core::instance()->kaduWindow());
}

void SingleWindow::addChatWidget(ChatWidget *chatWidget)
{
	m_tabs->addTab(chatWidget, chatWidget->icon(), QString());
	updateTabName(chatWidget);

	connect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
		this, SLOT(onChatKeyPressed(QKeyEvent *, CustomInput *, bool &)));

	connect(chatWidget, SIGNAL(unreadMessagesCountChanged(ChatWidget*)),
			this, SLOT(unreadMessagesCountChanged(ChatWidget*)));
	connect(chatWidget, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
	connect(chatWidget, SIGNAL(titleChanged(ChatWidget * , const QString &)),
			this, SLOT(onTitleChanged(ChatWidget *, const QString &)));
}

void SingleWindow::removeChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	chatWidget->setParent(nullptr);
	int index = m_tabs->indexOf(chatWidget);
	if (index >= 0)
		closeTab(index);
}

void SingleWindow::updateTabIcon(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	const int i = m_tabs->indexOf(chatWidget);
	if (-1 == i)
		return;

	if (chatWidget->unreadMessagesCount() > 0)
		m_tabs->setTabIcon(i, KaduIcon("protocols/common/message").icon());
	else
		m_tabs->setTabIcon(i, chatWidget->icon());
}

// TODO: share with tabs
void SingleWindow::updateTabName(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	const int i = m_tabs->indexOf(chatWidget);
	if (-1 == i)
		return;

	const Chat &chat = chatWidget->chat();
	QString baseTabName;
	if (!chat.display().isEmpty())
		baseTabName = chat.display();
	else
	{
		int contactsCount = chat.contacts().count();
		if (contactsCount > 1)
			baseTabName = tr("Conference [%1]").arg(contactsCount);
		else
			baseTabName = chat.name();
	}

	if (config_file.readBoolEntry("SingleWindow", "NumMessagesInTab", false) && chatWidget->unreadMessagesCount() > 0)
	{
		m_tabs->setTabText(i, QString("%1 [%2]").arg(baseTabName).arg(chatWidget->unreadMessagesCount()));
		m_tabs->setTabToolTip(i, QString("%1\n%2 new message(s)").arg(chatWidget->title()).arg(chatWidget->unreadMessagesCount()));
	}
	else
	{
		m_tabs->setTabText(i, baseTabName);
		m_tabs->setTabToolTip(i, chatWidget->title());
	}
}

void SingleWindow::closeTab(int index)
{
	ChatWidget *chatWidget = static_cast<ChatWidget *>(m_tabs->widget(index));

	disconnect(chatWidget->edit(), 0, this, 0);
	disconnect(chatWidget, 0, this, 0);

	chatWidget->requestClose();
	m_tabs->removeTab(index);
}

void SingleWindow::closeEvent(QCloseEvent *event)
{
	// do not block window closing when session is about to close
	if (Core::instance()->application()->isSavingSession())
	{
		QWidget::closeEvent(event);
		return;
	}

	if (Core::instance()->kaduWindow()->docked())
	{
		event->ignore();
		hide();
	}
	else
	{
		QWidget::closeEvent(event);
		Core::instance()->application()->quit();
	}
}

void SingleWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape && Core::instance()->kaduWindow()->docked())
	{
		hide();
		return;
	}

	QWidget::keyPressEvent(event);
}

void SingleWindow::resizeEvent(QResizeEvent *event)
{
	QSize newSize = event->size();
	m_split->resize(newSize);
}

void SingleWindow::tryActivateChatWidget(ChatWidget *chatWidget)
{
	int index = m_tabs->indexOf(chatWidget);
	if (index < 0)
		return;

	_activateWindow(this);

	m_tabs->setCurrentIndex(index);
	chatWidget->edit()->setFocus();
}

void SingleWindow::unreadMessagesCountChanged(ChatWidget *chatWidget)
{
	Q_ASSERT(chatWidget);

	updateTabIcon(chatWidget);
	updateTabName(chatWidget);
}

bool SingleWindow::isChatWidgetActive(const ChatWidget *chatWidget)
{
	return m_tabs->currentWidget() == chatWidget && _isWindowActiveOrFullyVisible(this);
}

void SingleWindow::onTabChange(int index)
{
	if (index == -1)
		return;

	ChatWidget *chatWidget = (ChatWidget *)m_tabs->widget(index);
	if (isChatWidgetActive(chatWidget))
		emit chatWidgetActivated(chatWidget);
}

void SingleWindow::onkaduKeyPressed(QKeyEvent *e)
{
	/* unfortunatelly does not work correctly */
	if (HotKey::shortCut(e, "ShortCuts", "FocusOnRosterTab"))
	{
		ChatWidget *chatWidget = static_cast<ChatWidget *>(m_tabs->currentWidget());
		if (chatWidget)
			chatWidget->edit()->setFocus();
	}
}

void SingleWindow::onChatKeyPressed(QKeyEvent *e, CustomInput *w, bool &handled)
{
	Q_UNUSED(w)

	/* workaround: we're receiving the same key event twice so ignore the duplicate */
	static int duplicate = 0;
	if (duplicate++)
	{
		duplicate = 0;
		handled = false;
		return;
	}

	handled = false;

	if (HotKey::shortCut(e, "ShortCuts", "SwitchTabLeft"))
	{
		int index = m_tabs->currentIndex();
		if (index > 0)
		{
			m_tabs->setCurrentIndex(index-1);
		}
		handled = true;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "SwitchTabRight"))
	{
		int index = m_tabs->currentIndex();
		if (index < m_tabs->count())
		{
			m_tabs->setCurrentIndex(index+1);
		}
		handled = true;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "HideShowRoster"))
	{
		QList<int> sizes = m_split->sizes();
		if (sizes[m_rosterPos] != 0)
			sizes[m_rosterPos] = 0;
		else
			sizes = m_splitSizes;
		m_split->setSizes(sizes);
		handled = true;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "FocusOnRosterTab"))
	{
		//kadu->userBox()->setFocus();//TODO: fixme
		handled = true;
	}
}

void SingleWindow::onIconChanged()
{
	ChatWidget *chatWidget = static_cast<ChatWidget *>(sender());
	updateTabIcon(chatWidget);
}

void SingleWindow::onTitleChanged(ChatWidget *chatWidget, const QString &newTitle)
{
	Q_UNUSED(newTitle)

	int chatIndex = m_tabs->indexOf(chatWidget);

	if (-1 == chatIndex || !chatWidget)
		return;

	updateTabName(chatWidget);
}

void SingleWindow::closeChat()
{
	QObject *chat = sender();
	if (chat)
		chat->deleteLater();
}

#include "moc_single-window.cpp"
