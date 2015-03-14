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

#include "accounts/account-manager.h"
#include "accounts/account.h"
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
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget-factory.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget-set-title.h"
#include "gui/widgets/chat-widget/chat-widget-title.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/toolbar.h"
#include "icons/kadu-icon.h"
#include "message/unread-message-repository.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "core/application.h"
#include "activate.h"
#include "debug.h"

#include "tabs.h"

static void disableNewTab(Action *action)
{
	if (action->context()->buddies().isAnyTemporary())
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(action->context()->chat());

	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "DefaultTabs"))
		action->setText(QCoreApplication::translate("TabsManager", "Chat in New Window"));
	else
		action->setText(QCoreApplication::translate("TabsManager", "Chat in New Tab"));

	kdebugf2();
}

TabsManager::TabsManager(QObject *parent) :
		ConfigurationUiHandler(parent), TargetTabs(-1)
{
	kdebugf();

	setState(StateNotLoaded);

	createDefaultConfiguration();

	TabDialog = new TabWidget(this);
	TabDialog->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TabDialog, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));
	connect(TabDialog, SIGNAL(contextMenu(QWidget *, const QPoint &)),
			this, SLOT(onContextMenu(QWidget *, const QPoint &)));

	Title = new ChatWidgetSetTitle{this};
	connect(Title, SIGNAL(titleChanged()), this, SLOT(onTitleChanged()));

	new WindowGeometryManager(new ConfigFileVariantWrapper("Chat", "TabWindowsGeometry"), QRect(30, 30, 550, 400), TabDialog);

	makePopupMenu();

	// pozycja tabów
	configurationUpdated();

	OpenInNewTabActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "openInNewTabAction",
		this, SLOT(onNewTab(QAction *, bool)),
		KaduIcon("internet-group-chat"), tr("Chat in New Tab"), false, disableNewTab
	);

	MenuInventory::instance()
		->menu("buddy-list")
		->addAction(OpenInNewTabActionDescription, KaduMenu::SectionChat, 20)
		->update();

	AttachToTabsActionDescription = new ActionDescription(this,
		ActionDescription::TypeChat, "attachToTabsAction",
		this, SLOT(onTabAttach(QAction *, bool)),
		KaduIcon("kadu_icons/tab-detach"), tr("Attach Chat to Tabs"), true
	);
	connect(AttachToTabsActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(attachToTabsActionCreated(Action *)));

	kdebugf2();
}

TabsManager::~TabsManager()
{
	kdebugf();

	MenuInventory::instance()
		->menu("buddy-list")
		->removeAction(OpenInNewTabActionDescription)
		->update();

	disconnect(Core::instance()->chatWidgetManager(), 0, this, 0);

	if (m_chatWidgetRepository)
		disconnect(m_chatWidgetRepository.data(), 0, this, 0);

	// jesli kadu nie konczy dzialania to znaczy ze modul zostal tylko wyladowany wiec odlaczamy rozmowy z kart
	if (!Core::instance()->isClosing())
		for (int i = TabDialog->count() - 1; i >= 0; i--)
			detachChat(static_cast<ChatWidget *>(TabDialog->widget(i)));

	delete TabDialog;
	TabDialog = 0;

	delete Menu;
	Menu = 0;

	kdebugf2();
}

void TabsManager::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (m_chatWidgetRepository)
	{
		connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetRemoved(ChatWidget*)),
				this, SLOT(onDestroyingChat(ChatWidget *)));
	}
}

void TabsManager::openStoredChatTabs()
{
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true))
		ensureLoaded();
}

void TabsManager::storeOpenedChatTabs()
{
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true))
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

	auto chatWidget = Core::instance()->chatWidgetFactory()->createChatWidget(chat, nullptr).release();
	setConfiguration(chatWidget);

	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true))
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
		_activateWindow(TabDialog);

	return chatWidget;
}

void TabsManager::removeChat(Chat chat)
{
	if (!chat)
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

void TabsManager::onNewTab(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	Chat chat = action->context()->chat();
	if (!chat)
		return;

	auto chatWidget = Core::instance()->chatWidgetRepository()->widgetForChat(chat);
	if (!chatWidget)
	{
		if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "DefaultTabs"))
		{
			chat.addProperty("tabs:tmp-detached", true, CustomProperties::NonStorable);
		}
		else
		{
			chat.addProperty("tabs:tmp-attached", true, CustomProperties::NonStorable);
		}
	}

	Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::Activate);

	chat.removeProperty("tabs:tmp-attached");
	chat.removeProperty("tabs:tmp-detached");

	kdebugf2();
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

	for (Action *action : AttachToTabsActionDescription->actions())
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

void TabsManager::onTabAttach(QAction *sender, bool toggled)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parentWidget());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	if (!toggled)
		detachChat(chatWidget);
	else
	{
		auto chat = chatWidget->chat();
		chat.removeProperty("tabs:detached");
		chat.addProperty("tabs:attached", true, CustomProperties::Storable);
		emit chatAcceptanceChanged(chat);
	}
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
	DetachTabMenuAction = Menu->addAction(KaduIcon("kadu_icons/tab-detach").icon(), tr("Detach"), this, SLOT(onMenuActionDetach()));
	Menu->addAction(tr("Detach all"), this, SLOT(onMenuActionDetachAll()));
	Menu->addSeparator();
	CloseTabMenuAction = Menu->addAction(KaduIcon("kadu_icons/tab-close").icon(), tr("Close"), this, SLOT(onMenuActionClose()));
	CloseOtherTabsMenuAction = Menu->addAction(tr("Close other tabs"), this, SLOT(onMenuActionCloseAllButActive()));
	CloseOtherTabsMenuAction->setEnabled(TabDialog->count() > 1);
	ReopenClosedTabMenuAction = Menu->addAction(tr("Reopen closed tab"), this, SLOT(reopenClosedChat()));
	ReopenClosedTabMenuAction->setEnabled(false);

	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Tabs", "OldStyleClosing"))
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

void TabsManager::attachToTabsActionCreated(Action *action)
{
	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(action->parentWidget());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	action->setChecked(TabDialog->indexOf(chatWidget) != -1);
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

		Chat chat = ChatManager::instance()->byUuid(chatId);
		if (!chat)
			continue;

		if (element.attribute("type") == "detachedChat")
			chat.addProperty("tabs:detached", true, CustomProperties::Storable);
		else if (element.attribute("type") == "tab")
			chat.addProperty("tabs:attached", true, CustomProperties::Storable);

		Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::DoNotActivate);
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
	return StorableObject::shouldStore() && Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "SaveOpenedWindows", true);
}

void TabsManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Q_UNUSED(mainConfigurationWindow)
}

void TabsManager::configurationUpdated()
{
	kdebugf();

	ConfigTabsBelowChats = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "TabsBelowChats");
	ConfigDefaultTabs = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "DefaultTabs");

	TabDialog->setTabPosition(ConfigTabsBelowChats ? QTabWidget::South : QTabWidget::North);

	TabDialog->configurationUpdated();

	DetachTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-detach").icon());
	CloseTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-close").icon());

	auto count = TabDialog->count();
	for (auto i = 0; i < count; i++)
		setConfiguration(static_cast<ChatWidget *>(TabDialog->widget(i)));

	kdebugf2();
}

void TabsManager::setConfiguration(ChatWidget* chatWidget)
{
	auto blinkChatTitle = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "BlinkChatTitle", false);
	chatWidget->title()->setBlinkIconWhenUnreadMessages(blinkChatTitle);
	chatWidget->title()->setBlinkTitleWhenUnreadMessages(blinkChatTitle);
	chatWidget->title()->setComposingStatePosition(ChatConfigurationHolder::instance()->composingStatePosition());
	chatWidget->title()->setShowUnreadMessagesCount(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "NewMessagesInChatTitle", false));
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

	Core::instance()->chatWidgetManager()->openChat(ClosedChats.takeFirst(), OpenChatActivation::Activate);
	ReopenClosedTabMenuAction->setEnabled(ClosedChats.isEmpty());
}

void TabsManager::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "MoveTabLeft", "Ctrl+Alt+Left");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "MoveTabRight", "Ctrl+Alt+Right");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "SwitchTabLeft", "Alt+Left");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "SwitchTabRight", "Alt+Right");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ShortCuts", "ReopenClosedTab", "Ctrl+Shift+T");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "TabsBelowChats", "false");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Chat", "DefaultTabs", "true");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Tabs", "CloseButton", "true");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Tabs", "OpenChatButton", "true");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Tabs", "OldStyleClosing", "false");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Tabs", "CloseButtonOnTab", "false");
}

#include "moc_tabs.cpp"
