/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
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

#include <QtGui/QApplication>
#include <QtGui/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/toolbar.h"
#include "icons/kadu-icon.h"
#include "message/unread-message-repository.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"

#include "activate.h"
#include "debug.h"

#include "tabs.h"

static void disableNewTab(Action *action)
{
	action->setEnabled(action->context()->chat());

	if (config_file.readBoolEntry("Chat", "DefaultTabs"))
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

	connect(&Timer, SIGNAL(timeout()), this, SLOT(onTimer()));

	TabDialog = new TabWidget(this);
	TabDialog->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TabDialog, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));
	connect(TabDialog, SIGNAL(contextMenu(QWidget *, const QPoint &)),
			this, SLOT(onContextMenu(QWidget *, const QPoint &)));

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

	Timer.stop();
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
	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		ensureLoaded();
}

void TabsManager::storeOpenedChatTabs()
{
	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		ensureStored();
}

bool TabsManager::acceptChatWidget(ChatWidget *chatWidget) const
{
	if (!chatWidget)
		return false;

	if (chatWidget->chat().property("tabs:detached", false).toBool())
		return false;

	if (ConfigDefaultTabs && (ConfigConferencesInTabs || chatWidget->chat().contacts().count() == 1))
	{
		if (TabDialog->count() > 0)
			return true;
		if ((NewChats.count() + 1) >= ConfigMinTabs)
			return true;
		return false;
	}
	else
		return true;
}

void TabsManager::addChatWidget(ChatWidget *chatWidget)
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chatWidget->chat().addProperty("tabs:fix2626", true, CustomProperties::Storable);

	if (chatWidget->chat().property("tabs:detached", false).toBool())
	{
		DetachedChats.append(chatWidget);
		return;
	}

	if (ConfigDefaultTabs && (ConfigConferencesInTabs || chatWidget->chat().contacts().count() == 1))
	{
		// jesli jest juz otwarte okno z kartami to dodajemy bezwzglednie nowe rozmowy do kart
		if (TabDialog->count() > 0)
		{
			insertTab(chatWidget);
		}
		else if ((NewChats.count() + 1) >= ConfigMinTabs)
		{
			foreach (ChatWidget *ch, NewChats)
			{
				// dodajemy karte tylko jesli jej jeszcze nie ma
				if (ch && TabDialog->indexOf(ch)==-1)
					insertTab(ch);
			}
			insertTab(chatWidget);
			NewChats.clear();
		}
		else
			NewChats.append(chatWidget);
	}
}

void TabsManager::removeChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	chatWidget->setParent(nullptr);
	auto index = TabDialog->indexOf(chatWidget);
	if (index >= 0)
		TabDialog->removeTab(index);
}

void TabsManager::onDestroyingChat(ChatWidget *chatWidget)
{
	kdebugf();

	chatWidget->chat().removeProperty("tabs:fix2626");

	if (TabDialog->indexOf(chatWidget) != -1)
		TabDialog->removeTab(TabDialog->indexOf(chatWidget));

	NewChats.removeAll(chatWidget);
	DetachedChats.removeAll(chatWidget);

	disconnect(chatWidget->edit(), 0, TabDialog, 0);
	disconnect(chatWidget, 0, this, 0);

	CloseOtherTabsMenuAction->setEnabled(TabDialog->count() > 1);

	ClosedChats.append(chatWidget->chat());
	ReopenClosedTabMenuAction->setEnabled(ClosedChats.size() > 0);

	kdebugf2();
}

void TabsManager::onIconChanged()
{
	ChatWidget *chatWidget = static_cast<ChatWidget *>(sender());
	if (chatWidget)
		updateTabIcon(chatWidget);
}

void TabsManager::onTitleChanged(ChatWidget *chatWidget, const QString &newTitle)
{
	kdebugf();

	int chatIndex = TabDialog->indexOf(chatWidget);

	if (-1 == chatIndex || !chatWidget)
		return;

	updateTabName(chatWidget);
	if (TabDialog->currentIndex() == chatIndex)
		TabDialog->setWindowTitle(newTitle);

	kdebugf2();
}

void TabsManager::onTabChange(int index)
{
	if (index < 0)
		return;

	ChatWidget *chatWidget = static_cast<ChatWidget *>(TabDialog->widget(index));

	const Chat &chat = chatWidget->chat();
	if (chat.unreadMessagesCount() > 0)
		emit chatWidgetActivated(chatWidget);

	TabDialog->setWindowTitle(chatWidget->title());
	TabDialog->setWindowIcon(chatWidget->icon());

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
	// exists - bring to front
	if (chatWidget)
	{
		if (TabDialog->indexOf(chatWidget) != -1)
			TabDialog->setCurrentWidget(chatWidget);
		_activateWindow(chatWidget);
	}
	else
	{
		// w miejsce recznego dodawania chata do kart automatyczne ;)
		if (!ConfigDefaultTabs && (chat.contacts().count() > 1) && !ConfigConferencesInTabs)
			chat.addProperty("tabs:detached", true, CustomProperties::Storable);

		Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::DoNotActivate);
	}

	kdebugf2();
}

void TabsManager::insertTab(ChatWidget *chatWidget)
{
	kdebugf();

	bool restoreChatGeometry = true;

	// jeśli jest otwarty chatwindow przypisany do chat to zostanie on zamknięty
	if (chatWidget->parentWidget())
	{
		chatWidget->parentWidget()->deleteLater();
		restoreChatGeometry = false;
	}

	ContactSet contacts = chatWidget->chat().contacts();

	DetachedChats.removeAll(chatWidget);

	for (Action *action : AttachToTabsActionDescription->actions())
	{
		if (action->context()->contacts() == contacts)
			action->setChecked(true);
	}

	// Ustawiam tytul karty w zaleznosci od tego czy mamy do czynienia z rozmowa czy z konferencja
	TabDialog->insertTab(TargetTabs, chatWidget, chatWidget->icon(), QString());

	if (restoreChatGeometry)
		chatWidget->kaduRestoreGeometry();

	updateTabName(chatWidget);

	TargetTabs = -1;

	connect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)),
			TabDialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));

	connect(chatWidget, SIGNAL(unreadMessagesCountChanged(ChatWidget*)),
			this, SLOT(unreadMessagesCountChanged(ChatWidget*)));
	connect(chatWidget, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
	connect(chatWidget, SIGNAL(titleChanged(ChatWidget * , const QString &)),
			this, SLOT(onTitleChanged(ChatWidget *, const QString &)));

	CloseOtherTabsMenuAction->setEnabled(TabDialog->count() > 1);

	kdebugf2();
}

// uff, troche dziwne to ale dziala tak jak trzeba
// TODO: review this!!!
void TabsManager::onTimer()
{
	kdebugf();
	static bool msg = true;

	bool tabsActive = _isWindowActiveOrFullyVisible(TabDialog);
	ChatWidget *currentChatWidget = static_cast<ChatWidget *>(TabDialog->currentWidget());

	auto unreadChatWidget = chatWidgetWithUnreadMessage();
	if (unreadChatWidget && !tabsActive)
	{
		qApp->alert(TabDialog);

		if (currentChatWidget->unreadMessagesCount() > 0)
		{
			if (ConfigBlinkChatTitle)
				TabDialog->setWindowTitle(msg
						? QString(currentChatWidget->title().length() + 5, ' ')
						: currentChatWidget->title());
			else if (ConfigShowNewMessagesNum)
				TabDialog->setWindowTitle('[' + QString::number(currentChatWidget->chat().unreadMessagesCount()) + "] " + currentChatWidget->title());
			else
				TabDialog->setWindowTitle(currentChatWidget->title());
		}
		else if (ConfigBlinkChatTitle && !msg)
			TabDialog->setWindowTitle(tr("NEW MESSAGE(S)"));
		else
			TabDialog->setWindowTitle(unreadChatWidget->title());
	}
	else
		TabDialog->setWindowTitle(currentChatWidget->title());

	msg = !msg;

	if (unreadChatWidget && !Timer.isActive())
		Timer.start(500);
	else if (!unreadChatWidget && Timer.isActive())
		Timer.stop();

	kdebugf2();
}

ChatWidget * TabsManager::chatWidgetWithUnreadMessage() const
{
	auto count = TabDialog->count();
	for (auto i = 0; i < count; i++)
	{
		auto chatWidget = qobject_cast<ChatWidget *>(TabDialog->widget(i));
		if (chatWidget)
		{
			auto chat = chatWidget->chat();
			if (chat.unreadMessagesCount() > 0)
				return chatWidget;
		}
	}

	return {};
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
		if (chatEditBox->actionContext()->contacts().count() != 1 && !ConfigConferencesInTabs)
			return;
		NewChats.clear();
		auto chat = chatWidget->chat();
		chat.removeProperty("tabs:detached");
		emit chatWidgetAcceptanceChanged(chatWidget);
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

	if (config_file.readBoolEntry("Tabs", "OldStyleClosing"))
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

	ContactSet contacts = action->context()->contacts();

	if (contacts.count() != 1 && !ConfigConferencesInTabs && TabDialog->indexOf(chatWidget) == -1)
		action->setEnabled(false);

	action->setChecked(TabDialog->indexOf(chatWidget) != -1);
}

void TabsManager::detachChat(ChatWidget *chatWidget)
{
	kdebugf();
	if (TabDialog->indexOf(chatWidget) == -1)
		return;

	auto chat = chatWidget->chat();
	chat.addProperty("tabs:detached", true, CustomProperties::Storable);
	emit chatWidgetAcceptanceChanged(chatWidget);
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

		Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::DoNotActivate);
	}
}

void TabsManager::store()
{
	if (!isValidStorage())
		return;

	XmlConfigFile *storageFile = storage()->storage();
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

		if ((TabDialog->indexOf(chatWidget) == -1) && (DetachedChats.indexOf(chatWidget) == -1))
			continue;

		QDomElement window_elem = storageFile->createElement(point, "Tab");

		window_elem.setAttribute("chat", chat.uuid().toString());
		if (TabDialog->indexOf(chatWidget) != -1)
			window_elem.setAttribute("type", "tab");
		else if (DetachedChats.indexOf(chatWidget) != -1)
			window_elem.setAttribute("type", "detachedChat");
	}
}

bool TabsManager::shouldStore()
{
	return StorableObject::shouldStore() && config_file.readBoolEntry("Chat", "SaveOpenedWindows", true);
}

void TabsManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Q_UNUSED(mainConfigurationWindow)
}

void TabsManager::configurationUpdated()
{
	kdebugf();
	ConfigConferencesInTabs = config_file.readBoolEntry("Chat", "ConferencesInTabs");
	ConfigTabsBelowChats = config_file.readBoolEntry("Chat", "TabsBelowChats");
	ConfigDefaultTabs = config_file.readBoolEntry("Chat", "DefaultTabs");
	ConfigMinTabs = config_file.readNumEntry("Chat", "MinTabs");
	ConfigBlinkChatTitle = config_file.readBoolEntry("Chat", "BlinkChatTitle");
	ConfigShowNewMessagesNum = config_file.readBoolEntry("Chat", "NewMessagesInChatTitle");

	TabDialog->setTabPosition(ConfigTabsBelowChats ? QTabWidget::South : QTabWidget::North);

	TabDialog->configurationUpdated();

	DetachTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-detach").icon());
	CloseTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-close").icon());

	kdebugf2();
}

void TabsManager::updateTabTextAndTooltip(int index, const QString &text, const QString &tooltip)
{
	if (TabDialog->tabText(index) != text)
		TabDialog->setTabText(index, text);
	if (TabDialog->tabToolTip(index) != tooltip)
		TabDialog->setTabToolTip(index, tooltip);
}

// TODO: share with single_window
void TabsManager::updateTabName(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	auto const index = TabDialog->indexOf(chatWidget);
	if (-1 == index)
		return;

	auto chatTitle = shortChatTitle(defaultChatTitle(chatWidget->chat()));
	if (chatWidget->unreadMessagesCount() > 0)
		updateTabTextAndTooltip(index, QString("%1 [%2]").arg(chatTitle).arg(chatWidget->unreadMessagesCount()),
				QString("%1\n%2 new message(s)").arg(chatWidget->title()).arg(chatWidget->unreadMessagesCount()));
	else
		updateTabTextAndTooltip(index, chatTitle, chatTitle);
}

QString TabsManager::defaultChatTitle(const Chat &chat) const
{
	if (!chat.display().isEmpty())
		return chat.display();

	auto contactsCount = chat.contacts().count();
	return contactsCount > 1
			? tr("Conference [%1]").arg(contactsCount)
			: chat.name();
}

QString TabsManager::shortChatTitle(const QString &chatTitle) const
{
	auto const tabTextLimit = 15;
	return  chatTitle.length() > tabTextLimit
			? chatTitle.left(tabTextLimit) + "..."
			: chatTitle;
}

void TabsManager::updateTabIcon(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	const int i = TabDialog->indexOf(chatWidget);
	if (-1 == i)
		return;

	if (chatWidget->unreadMessagesCount() > 0)
		TabDialog->setTabIcon(i, KaduIcon("protocols/common/message").icon());
	else
		TabDialog->setTabIcon(i, chatWidget->icon());

	if (TabDialog->currentIndex() == i)
		TabDialog->setWindowIcon(TabDialog->tabIcon(i));
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
	config_file.addVariable("ShortCuts", "MoveTabLeft", "Ctrl+Alt+Left");
	config_file.addVariable("ShortCuts", "MoveTabRight", "Ctrl+Alt+Right");
	config_file.addVariable("ShortCuts", "SwitchTabLeft", "Alt+Left");
	config_file.addVariable("ShortCuts", "SwitchTabRight", "Alt+Right");
	config_file.addVariable("ShortCuts", "ReopenClosedTab", "Ctrl+Shift+T");
	config_file.addVariable("Chat", "ConferencesInTabs", "true");
	config_file.addVariable("Chat", "TabsBelowChats", "false");
	config_file.addVariable("Chat", "DefaultTabs", "true");
	config_file.addVariable("Chat", "MinTabs", "1");
	config_file.addVariable("Tabs", "CloseButton", "true");
	config_file.addVariable("Tabs", "OpenChatButton", "true");
	config_file.addVariable("Tabs", "OldStyleClosing", "false");
	config_file.addVariable("Tabs", "CloseButtonOnTab", "false");
}

void TabsManager::unreadMessagesCountChanged(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	updateTabIcon(chatWidget);
	updateTabName(chatWidget);

	if (chatWidget->unreadMessagesCount() > 0 && !Timer.isActive())
		QMetaObject::invokeMethod(this, "onTimer", Qt::QueuedConnection);
}

#include "moc_tabs.cpp"
