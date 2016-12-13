/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright Michal Podsiadlik (michal at gov.one.pl)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <injeqt/injeqt.h>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "actions/action-description.h"
#include "actions/action.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "core/session-service.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "icons/kadu-icon.h"
#include "menu/menu-inventory.h"
#include "message/unread-message-repository.h"
#include "os/generic/window-geometry-manager.h"
#include "plugin/plugin-injected-factory.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-widget/chat-widget-impl.h"
#include "widgets/chat-widget/chat-widget-manager.h"
#include "widgets/chat-widget/chat-widget-repository.h"
#include "widgets/chat-widget/chat-widget-set-title.h"
#include "widgets/chat-widget/chat-widget-title.h"
#include "widgets/configuration/configuration-widget.h"
#include "widgets/toolbar.h"
#include "activate.h"
#include "debug.h"

#include "attach-tab-action.h"
#include "open-in-new-tab-action.h"

#include "tabs.h"

TabsManager::TabsManager(QObject *parent) :
		StorableObject(parent), TargetTabs(-1)
{
	Title = new ChatWidgetSetTitle{this};
}

TabsManager::~TabsManager()
{
}

void TabsManager::setAttachTabAction(AttachTabAction *attachTabAction)
{
	m_attachTabAction = attachTabAction;
}

void TabsManager::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
	m_chatConfigurationHolder = chatConfigurationHolder;
}

void TabsManager::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void TabsManager::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void TabsManager::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void TabsManager::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void TabsManager::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void TabsManager::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void TabsManager::setOpenInNewTabAction(OpenInNewTabAction *openInNewTabAction)
{
	m_openInNewTabAction = openInNewTabAction;
}

void TabsManager::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void TabsManager::setSessionService(SessionService *sessionService)
{
	m_sessionService = sessionService;
}

void TabsManager::init()
{
	kdebugf();

	setState(StateNotLoaded);

	createDefaultConfiguration();

	connect(m_chatWidgetRepository, SIGNAL(chatWidgetRemoved(ChatWidget*)), this, SLOT(onDestroyingChat(ChatWidget *)));

	TabDialog = m_pluginInjectedFactory->makeInjected<TabWidget>(this);
	TabDialog->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TabDialog, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));
	connect(TabDialog, SIGNAL(contextMenu(QWidget *, const QPoint &)),
			this, SLOT(onContextMenu(QWidget *, const QPoint &)));

	connect(Title, SIGNAL(titleChanged()), this, SLOT(onTitleChanged()));

	new WindowGeometryManager(new ConfigFileVariantWrapper(m_configuration, "Chat", "TabWindowsGeometry"), QRect(30, 30, 550, 400), TabDialog);

	makePopupMenu();

	// pozycja tabów
	configurationUpdated();

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_openInNewTabAction, KaduMenu::SectionChat, 20)
		->update();

	openStoredChatTabs();

	kdebugf2();
}

void TabsManager::done()
{
	kdebugf();

	storeOpenedChatTabs();

	m_menuInventory
		->menu("buddy-list")
		->removeAction(m_openInNewTabAction)
		->update();

	disconnect(m_chatWidgetManager, 0, this, 0);

	if (m_chatWidgetRepository)
		disconnect(m_chatWidgetRepository.data(), 0, this, 0);

	// jesli kadu nie konczy dzialania to znaczy ze modul zostal tylko wyladowany wiec odlaczamy rozmowy z kart
	if (!m_sessionService->isClosing())
		for (int i = TabDialog->count() - 1; i >= 0; i--)
			detachChat(static_cast<ChatWidget *>(TabDialog->widget(i)));

	m_closing = true;
	delete TabDialog;
	TabDialog = 0;

	delete Menu;
	Menu = 0;

	kdebugf2();
}

void TabsManager::openStoredChatTabs()
{
	if (m_configuration->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true))
		ensureLoaded();
}

void TabsManager::storeOpenedChatTabs()
{
	if (m_configuration->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true))
		ensureStored();
}

bool TabsManager::acceptChat(Chat chat) const
{
	if (!chat)
		return false;

	if (chat.property("tabs:tmp-attached", false).toBool())
		return true;
	if (chat.property("tabs:tmp-detached", false).toBool())
		return false;
	if (chat.property("tabs:attached", false).toBool())
		return true;
	if (chat.property("tabs:detached", false).toBool())
		return false;

	return ConfigDefaultTabs;
}

ChatWidget * TabsManager::addChat(Chat chat, OpenChatActivation activation)
{
	kdebugf();

	auto chatWidget = m_pluginInjectedFactory->makeInjected<ChatWidgetImpl>(chat, nullptr);
	setConfiguration(chatWidget);

	if (m_configuration->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true))
		chatWidget->chat().addProperty("tabs:fix2626", true, CustomProperties::Storable);

	auto tmpAttached = chat.property("tabs:tmp-attached", false).toBool();
	auto tmpDetached =  chat.property("tabs:tmp-detached", false).toBool();
	auto attached = chat.property("tabs:attached", false).toBool();
	auto detached = chat.property("tabs:detached", false).toBool();

	if (!tmpAttached && tmpDetached)
	{
		DetachedChats.append(chat);
		return chatWidget;
	}

	if (tmpAttached)
	{
		insertTab(chatWidget);
	}
	else if (!attached && detached)
	{
		DetachedChats.append(chat);
	}
	else if (attached || ConfigDefaultTabs)
		insertTab(chatWidget);

	if (TabDialog->count() == 1) // first tab
	{
		switch (activation)
		{
			case OpenChatActivation::Minimize:
				TabDialog->showMinimized();
				break;
			default:
				TabDialog->show();
				break;
		}
	}

	if (activation == OpenChatActivation::Activate)
		_activateWindow(m_configuration, TabDialog);

	return chatWidget;
}

void TabsManager::removeChat(Chat chat)
{
	// TODO: fix this whole thing
	if (!chat || m_closing)
		return;

	auto count = TabDialog->count();
	for (auto i = 0; i < count; i++)
	{
		auto chatWidget = qobject_cast<ChatWidget *>(TabDialog->widget(i));
		if (chatWidget && chatWidget->chat() == chat)
		{
			TabDialog->removeTab(i);
			chatWidget->deleteLater();
			return;
		}
	}
}

void TabsManager::onDestroyingChat(ChatWidget *chatWidget)
{
	kdebugf();

	chatWidget->chat().removeProperty("tabs:fix2626");

	if (TabDialog->indexOf(chatWidget) != -1)
		TabDialog->removeTab(TabDialog->indexOf(chatWidget));

	DetachedChats.removeAll(chatWidget->chat());

	disconnect(chatWidget->edit(), 0, TabDialog, 0);
	disconnect(chatWidget, 0, this, 0);

	CloseOtherTabsMenuAction->setEnabled(TabDialog->count() > 1);

	ClosedChats.append(chatWidget->chat());
	ReopenClosedTabMenuAction->setEnabled(ClosedChats.size() > 0);

	kdebugf2();
}

void TabsManager::onTitleChanged(ChatWidget *chatWidget)
{
	kdebugf();

	int chatIndex = TabDialog->indexOf(chatWidget);

	if (-1 == chatIndex || !chatWidget)
		return;

	updateTabTitle(chatWidget);
	//if (TabDialog->currentIndex() == chatIndex)
		//TabDialog->setWindowTitle(TabDialog->tabText(chatIndex));

	kdebugf2();
}

void TabsManager::onTitleChanged()
{
	TabDialog->setWindowTitle(Title->fullTitle());
	TabDialog->setWindowIcon(Title->icon());
}

void TabsManager::onTabChange(int index)
{
	if (index < 0)
		return;

	ChatWidget *chatWidget = static_cast<ChatWidget *>(TabDialog->widget(index));
	Title->setActiveChatWidget(chatWidget);

	const Chat &chat = chatWidget->chat();
	if (chat.unreadMessagesCount() > 0)
		emit chatWidgetActivated(chatWidget);

	//TabDialog->setWindowTitle(chatWidget->title()->title());
	//TabDialog->setWindowIcon(chatWidget->title()->icon());

	chatWidget->edit()->setFocus();
}

void TabsManager::insertTab(ChatWidget *chatWidget)
{
	kdebugf();

	Title->addChatWidget(chatWidget);

	bool restoreChatGeometry = true;

	// jeśli jest otwarty chatwindow przypisany do chat to zostanie on zamknięty
	if (chatWidget->parentWidget())
	{
		chatWidget->parentWidget()->deleteLater();
		restoreChatGeometry = false;
	}

	ContactSet contacts = chatWidget->chat().contacts();

	DetachedChats.removeAll(chatWidget->chat());

	for (Action *action : m_attachTabAction->actions())
	{
		if (action->context()->contacts() == contacts)
			action->setChecked(true);
	}

	// Ustawiam tytul karty w zaleznosci od tego czy mamy do czynienia z rozmowa czy z konferencja
	TabDialog->insertTab(TargetTabs, chatWidget, chatWidget->title()->blinkingIcon(), QString());

	if (restoreChatGeometry)
		chatWidget->kaduRestoreGeometry();

	updateTabTitle(chatWidget);

	TargetTabs = -1;

	connect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)),
			TabDialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));

	connect(chatWidget->title(), SIGNAL(titleChanged(ChatWidget*)), this, SLOT(onTitleChanged(ChatWidget*)));

	CloseOtherTabsMenuAction->setEnabled(TabDialog->count() > 1);

	// unreadMessagesCountChanged(chatWidget);
	if (_isActiveWindow(TabDialog) && TabDialog->currentWidget() == chatWidget)
		emit chatWidgetActivated(chatWidget);

	kdebugf2();
}

void TabsManager::onContextMenu(QWidget *w, const QPoint &pos)
{
	kdebugf();
	SelectedChat = qobject_cast<ChatWidget *>(w);
	Menu->popup(pos);
	kdebugf2();
}

void TabsManager::makePopupMenu()
{
	kdebugf();

	Menu = new QMenu();
	DetachTabMenuAction = Menu->addAction(m_iconsManager->iconByPath(KaduIcon("kadu_icons/tab-detach")), tr("Detach"), this, SLOT(onMenuActionDetach()));
	Menu->addAction(tr("Detach all"), this, SLOT(onMenuActionDetachAll()));
	Menu->addSeparator();
	CloseTabMenuAction = Menu->addAction(m_iconsManager->iconByPath(KaduIcon("kadu_icons/tab-close")), tr("Close"), this, SLOT(onMenuActionClose()));
	CloseOtherTabsMenuAction = Menu->addAction(tr("Close other tabs"), this, SLOT(onMenuActionCloseAllButActive()));
	CloseOtherTabsMenuAction->setEnabled(TabDialog->count() > 1);
	ReopenClosedTabMenuAction = Menu->addAction(tr("Reopen closed tab"), this, SLOT(reopenClosedChat()));
	ReopenClosedTabMenuAction->setEnabled(false);

	if (m_configuration->deprecatedApi()->readBoolEntry("Tabs", "OldStyleClosing"))
		Menu->addAction(tr("Close all"), this, SLOT(onMenuActionCloseAll()));

	kdebugf2();
}

void TabsManager::onMenuActionDetach()
{
	detachChat(SelectedChat);
}

void TabsManager::onMenuActionDetachAll()
{
	for (int i = TabDialog->count()-1; i >= 0; --i)
		detachChat(static_cast<ChatWidget *>(TabDialog->widget(i)));
}

void TabsManager::onMenuActionClose()
{
	delete SelectedChat;
}

void TabsManager::onMenuActionCloseAll()
{
	for (int i = TabDialog->count() - 1; i >= 0; --i)
		delete TabDialog->widget(i);
}

void TabsManager::onMenuActionCloseAllButActive()
{
	int activeTabIndex = TabDialog->indexOf(SelectedChat);
	if (activeTabIndex == -1)
		return;

	for (int i = TabDialog->count() - 1; i >= 0; --i)
	{
		if (i != activeTabIndex)
			delete TabDialog->widget(i);
	}
}

void TabsManager::attachChat(ChatWidget *chatWidget)
{
	auto chat = chatWidget->chat();
	chat.removeProperty("tabs:detached");
	chat.addProperty("tabs:attached", true, CustomProperties::Storable);

	emit chatAcceptanceChanged(chat);
}

void TabsManager::detachChat(ChatWidget *chatWidget)
{
	kdebugf();
	if (TabDialog->indexOf(chatWidget) == -1)
		return;

	auto chat = chatWidget->chat();
	chat.addProperty("tabs:detached", true, CustomProperties::Storable);
	chat.removeProperty("tabs:attached");
	emit chatAcceptanceChanged(chat);
}

void TabsManager::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	QDomElement itemsNode = storage()->point();
	if (itemsNode.isNull())
		return;

	QVector<QDomElement> itemElements = storage()->storage()->getNodes(itemsNode, "Tab");
	foreach (const QDomElement &element, itemElements)
	{
		QUuid chatId(element.attribute("chat"));

		if (chatId.isNull())
			continue;

		Chat chat = m_chatManager->byUuid(chatId);
		if (!chat)
			continue;

		if (element.attribute("type") == "detachedChat")
			chat.addProperty("tabs:detached", true, CustomProperties::Storable);
		else if (element.attribute("type") == "tab")
			chat.addProperty("tabs:attached", true, CustomProperties::Storable);

		m_chatWidgetManager->openChat(chat, OpenChatActivation::DoNotActivate);
	}
}

void TabsManager::store()
{
	if (!isValidStorage())
		return;

	ConfigurationApi *storageFile = storage()->storage();
	QDomElement point = storage()->point();

	storageFile->removeChildren(point);

	if (!m_chatWidgetRepository)
		return;

	for (auto *chatWidget : m_chatWidgetRepository.data())
	{
		if (!chatWidget)
			continue;

		Chat chat = chatWidget->chat();

		if (!chat)
			continue;

		if ((TabDialog->indexOf(chatWidget) == -1) && (DetachedChats.indexOf(chat) == -1))
			continue;

		QDomElement window_elem = storageFile->createElement(point, "Tab");

		window_elem.setAttribute("chat", chat.uuid().toString());
		if (TabDialog->indexOf(chatWidget) != -1)
			window_elem.setAttribute("type", "tab");
		else if (DetachedChats.indexOf(chat) != -1)
			window_elem.setAttribute("type", "detachedChat");
	}
}

bool TabsManager::shouldStore()
{
	return StorableObject::shouldStore() && m_configuration->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true);
}

void TabsManager::configurationUpdated()
{
	kdebugf();

	ConfigTabsBelowChats = m_configuration->deprecatedApi()->readBoolEntry("Chat", "TabsBelowChats");
	ConfigDefaultTabs = m_configuration->deprecatedApi()->readBoolEntry("Chat", "DefaultTabs");

	TabDialog->setTabPosition(ConfigTabsBelowChats ? QTabWidget::South : QTabWidget::North);

	TabDialog->configurationUpdated();

	DetachTabMenuAction->setIcon(m_iconsManager->iconByPath(KaduIcon("kadu_icons/tab-detach")));
	CloseTabMenuAction->setIcon(m_iconsManager->iconByPath(KaduIcon("kadu_icons/tab-close")));

	auto count = TabDialog->count();
	for (auto i = 0; i < count; i++)
		setConfiguration(static_cast<ChatWidget *>(TabDialog->widget(i)));

	kdebugf2();
}

void TabsManager::setConfiguration(ChatWidget* chatWidget)
{
	auto blinkChatTitle = m_configuration->deprecatedApi()->readBoolEntry("Chat", "BlinkChatTitle", false);
	chatWidget->title()->setBlinkIconWhenUnreadMessages(blinkChatTitle);
	chatWidget->title()->setBlinkTitleWhenUnreadMessages(blinkChatTitle);
	chatWidget->title()->setComposingStatePosition(m_chatConfigurationHolder->composingStatePosition());
	chatWidget->title()->setShowUnreadMessagesCount(m_configuration->deprecatedApi()->readBoolEntry("Chat", "NewMessagesInChatTitle", false));
}

// TODO: share with single_window
void TabsManager::updateTabTitle(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	auto const index = TabDialog->indexOf(chatWidget);
	if (-1 == index)
		return;

	TabDialog->setTabText(index, chatWidget->title()->shortTitle());
	TabDialog->setTabToolTip(index, chatWidget->title()->tooltip());
	TabDialog->setTabIcon(index, chatWidget->title()->blinkingIcon());
	if (TabDialog->currentIndex() == index)
		TabDialog->setWindowIcon(TabDialog->tabIcon(index));
}

void TabsManager::closeChat()
{
	QObject *chat = sender();
	if (chat)
		chat->deleteLater();
}

void TabsManager::reopenClosedChat()
{
	if (ClosedChats.isEmpty())
		return;

	m_chatWidgetManager->openChat(ClosedChats.takeFirst(), OpenChatActivation::Activate);
	ReopenClosedTabMenuAction->setEnabled(ClosedChats.isEmpty());
}

void TabsManager::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("ShortCuts", "MoveTabLeft", "Ctrl+Alt+Left");
	m_configuration->deprecatedApi()->addVariable("ShortCuts", "MoveTabRight", "Ctrl+Alt+Right");
	m_configuration->deprecatedApi()->addVariable("ShortCuts", "SwitchTabLeft", "Alt+Left");
	m_configuration->deprecatedApi()->addVariable("ShortCuts", "SwitchTabRight", "Alt+Right");
	m_configuration->deprecatedApi()->addVariable("ShortCuts", "ReopenClosedTab", "Ctrl+Shift+T");
	m_configuration->deprecatedApi()->addVariable("Chat", "TabsBelowChats", "false");
	m_configuration->deprecatedApi()->addVariable("Chat", "DefaultTabs", "true");
	m_configuration->deprecatedApi()->addVariable("Tabs", "CloseButton", "true");
	m_configuration->deprecatedApi()->addVariable("Tabs", "OpenChatButton", "true");
	m_configuration->deprecatedApi()->addVariable("Tabs", "OldStyleClosing", "false");
	m_configuration->deprecatedApi()->addVariable("Tabs", "CloseButtonOnTab", "false");
}

#include "moc_tabs.cpp"
