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
#include "configuration/configuration-manager.h"
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
		action->setText(qApp->translate("TabsManager", "Chat in New Window"));
	else
		action->setText(qApp->translate("TabsManager", "Chat in New Tab"));

	kdebugf2();
}

TabsManager::TabsManager(QObject *parent) :
		ConfigurationUiHandler(parent), ForceTabs(false), TargetTabs(-1)
{
	kdebugf();

	setState(StateNotLoaded);
	ConfigurationManager::instance()->registerStorableObject(this);

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
		->addAction(OpenInNewTabActionDescription, KaduMenu::SectionChat, 20);

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
		->removeAction(OpenInNewTabActionDescription);

	Timer.stop();
	disconnect(ChatWidgetManager::instance(), 0, this, 0);

	if (m_chatWidgetRepository)
		disconnect(m_chatWidgetRepository.data(), 0, this, 0);

	// Call it before we close tab window.
	ConfigurationManager::instance()->unregisterStorableObject(this);

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

bool TabsManager::containChatWidget(ChatWidget *chatWidget)
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chatWidget->chat().addProperty("tabs:fix2626", true, CustomProperties::Storable);

	if (chatWidget->chat().property("tabs:detached", false).toBool())
	{
		DetachedChats.append(chatWidget);
		return false;
	}

	// jesli chat ma zostac bezwzglednie dodany do kart np w wyniku wyboru w menu
	if (ForceTabs)
	{
		ForceTabs = false;
		insertTab(chatWidget);
		return true;
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
		{
			NewChats.append(chatWidget);
			return false;
		}
	}

	return true;
}

void TabsManager::onDestroyingChat(ChatWidget *chatWidget)
{
	kdebugf();

	chatWidget->chat().removeProperty("tabs:fix2626");

	if (TabDialog->indexOf(chatWidget) != -1)
	{
		// zapamietuje wewnetrzne rozmiary chata
		chatWidget->kaduStoreGeometry();

		TabDialog->removeTab(TabDialog->indexOf(chatWidget));
	}

	NewChats.removeAll(chatWidget);
	DetachedChats.removeAll(chatWidget);

	removeChatWidgetFromChatWidgetsWithMessage(chatWidget);

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
	{
		chatWidget->markActive();
		updateTabName(chatWidget);

		removeChatWidgetFromChatWidgetsWithMessage(chatWidget);
	}

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

	auto chatWidget = ChatWidgetManager::instance()->byChat(chat);
	// exists - bring to front
	if (chatWidget)
	{
		if (TabDialog->indexOf(chatWidget) != -1)
			TabDialog->setCurrentWidget(chatWidget);
		_activateWindow(chatWidget);
	}
	else
	{
		if (ConfigDefaultTabs)
			chat.addProperty("tabs:detached", true, CustomProperties::Storable);
		// w miejsce recznego dodawania chata do kart automatyczne ;)
		else if (chat.contacts().count() == 1 || ConfigConferencesInTabs)
			ForceTabs = true;

		ChatWidgetManager::instance()->openChat(chat, OpenChatActivation::DoNotActivate);
	}

	kdebugf2();
}

void TabsManager::insertTab(ChatWidget *chatWidget)
{
	kdebugf();

	chatWidget->chat().removeProperty("tabs:detached");

	bool restoreChatGeometry = true;

	// jeśli jest otwarty chatwindow przypisany do chat to zostanie on zamknięty
	if (chatWidget->parentWidget())
	{
		chatWidget->parentWidget()->deleteLater();
		restoreChatGeometry = false;
	}

	ContactSet contacts = chatWidget->chat().contacts();

	DetachedChats.removeAll(chatWidget);

	foreach (Action *action, AttachToTabsActionDescription->actions())
	{
		if (action->context()->contacts() == contacts)
			action->setChecked(true);
	}

	chatWidget->setContainer(TabDialog);

	// Ustawiam tytul karty w zaleznosci od tego czy mamy do czynienia z rozmowa czy z konferencja
	TabDialog->insertTab(TargetTabs, chatWidget, chatWidget->icon(), QString());

	if (restoreChatGeometry)
		chatWidget->kaduRestoreGeometry();

	updateTabName(chatWidget);

	TargetTabs = -1;

	connect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)),
			TabDialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));
	connect(chatWidget, SIGNAL(closed()), this, SLOT(closeChat()));
	connect(chatWidget, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
	connect(chatWidget, SIGNAL(titleChanged(ChatWidget * , const QString &)),
			this, SLOT(onTitleChanged(ChatWidget *, const QString &)));

	CloseOtherTabsMenuAction->setEnabled(TabDialog->count() > 1);

	kdebugf2();
}

void TabsManager::addChatWidgetToChatWidgetsWithMessage(ChatWidget *chatWidget)
{
	if (ChatsWithNewMessages.contains(chatWidget))
		return;

	ChatsWithNewMessages.append(chatWidget);
	updateTabIcon(chatWidget);

	if (!Timer.isActive())
		QMetaObject::invokeMethod(this, "onTimer", Qt::QueuedConnection);
}

void TabsManager::removeChatWidgetFromChatWidgetsWithMessage(ChatWidget *chatWidget)
{
	if (!ChatsWithNewMessages.contains(chatWidget))
		return;

	ChatsWithNewMessages.removeAll(chatWidget);
	updateTabIcon(chatWidget);
}

// uff, troche dziwne to ale dziala tak jak trzeba
// TODO: review this!!!
void TabsManager::onTimer()
{
	kdebugf();
	ChatWidget *chatWidget;
	static bool msg = true;

	bool tabsActive = _isWindowActiveOrFullyVisible(TabDialog);
	ChatWidget *currentChatWidget = static_cast<ChatWidget *>(TabDialog->currentWidget());

	for (int i = TabDialog->count() -1; i >= 0; i--)
	{
		chatWidget = static_cast<ChatWidget *>(TabDialog->widget(i));

		// czy trzeba cos robia ?
		if (ChatsWithNewMessages.contains(chatWidget))
		{
			if (tabsActive)
			{
				if (currentChatWidget == chatWidget)
				{
					chatWidget->markActive();
					removeChatWidgetFromChatWidgetsWithMessage(chatWidget);
				}

				TabDialog->setWindowTitle(currentChatWidget->title());
			}
			else
			{
				qApp->alert(TabDialog);
				// jesli chat jest na aktywnej karcie - zachowuje sie jak normalne okno
				if (currentChatWidget == chatWidget)
				{
					if (msg)
					{
						if (ConfigBlinkChatTitle)
							TabDialog->setWindowTitle(QString(chatWidget->title().length() + 5, ' '));
					}
					else
					{
						if (ConfigShowNewMessagesNum)
							TabDialog->setWindowTitle('[' + QString::number(chatWidget->chat().unreadMessagesCount()) + "] " + chatWidget->title());
						else
							TabDialog->setWindowTitle(chatWidget->title());
					}
				}
				// jesli nie w zaleznosci od konfiguracji wystepuje "miganie" lub nie
				else if (ConfigBlinkChatTitle && !msg)
					TabDialog->setWindowTitle(tr("NEW MESSAGE(S)"));
				else
					TabDialog->setWindowTitle(chatWidget->title());
			}

			updateTabName(chatWidget);
		}
	}

	msg = !msg;

	if (!ChatsWithNewMessages.isEmpty() && !Timer.isActive())
		Timer.start(500);
	else if (ChatsWithNewMessages.isEmpty() && Timer.isActive())
		Timer.stop();

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
		if (chatEditBox->actionContext()->contacts().count() != 1 && !ConfigConferencesInTabs)
			return;
		NewChats.clear();
		insertTab(chatWidget);
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

bool TabsManager::detachChat(ChatWidget *chatWidget)
{
	kdebugf();
	if (TabDialog->indexOf(chatWidget) == -1)
		return false;
	auto chat = chatWidget->chat();
	delete chatWidget;

	// omg this is bad
	chat.addProperty("tabs:detached", true, CustomProperties::Storable);
	ChatWidgetManager::instance()->openChat(chat, OpenChatActivation::Activate);
	return true;
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

		auto chatWidget = ChatWidgetManager::instance()->byChat(chat);
		if (!chatWidget)
		{
			if (element.attribute("type") == "tab")
				ForceTabs = true;
			else if (element.attribute("type") == "detachedChat")
				chat.addProperty("tabs:detached", true, CustomProperties::Storable);
			ChatWidgetManager::instance()->openChat(chat, OpenChatActivation::DoNotActivate);
		}
		else if (element.attribute("type") == "tab")
			insertTab(chatWidget);
		else if (element.attribute("type") == "detachedChat")
			DetachedChats.append(chatWidget);
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

	foreach (ChatWidget *chatWidget, m_chatWidgetRepository.data()->widgets())
	{
		if (!chatWidget)
			continue;

		Chat chat = chatWidget->chat();

		if (!chat)
			continue;

		if ((TabDialog->indexOf(chatWidget) == -1) && (DetachedChats.indexOf(chatWidget) == -1))
			continue;

		chatWidget->kaduStoreGeometry();

		QDomElement window_elem = storageFile->createElement(point, "Tab");

		window_elem.setAttribute("chat", chat.uuid());
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
	ConfigMinTabs = config_file.readUnsignedNumEntry("Chat", "MinTabs");
	ConfigBlinkChatTitle = config_file.readBoolEntry("Chat", "BlinkChatTitle");
	ConfigShowNewMessagesNum = config_file.readBoolEntry("Chat", "NewMessagesInChatTitle");

	TabDialog->setTabPosition(ConfigTabsBelowChats ? QTabWidget::South : QTabWidget::North);

	TabDialog->configurationUpdated();

	DetachTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-detach").icon());
	CloseTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-close").icon());

	kdebugf2();
}

void TabsManager::setTabTextAndTooltipIfDiffer(int index, const QString &text, const QString &tooltip)
{
	if (TabDialog->tabText(index) != text)
		TabDialog->setTabText(index, text);
	if (TabDialog->tabToolTip(index) != tooltip)
		TabDialog->setTabToolTip(index, tooltip);
}

// TODO: share with single_window
void TabsManager::updateTabName(ChatWidget *chatWidget)
{
	const int i = TabDialog->indexOf(chatWidget);
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

	int tabTextLimit = 15;
	baseTabName = baseTabName.length() > tabTextLimit
			? baseTabName.left(tabTextLimit) + "..."
			: baseTabName;

	if (chat.unreadMessagesCount() > 0)
		setTabTextAndTooltipIfDiffer(i, QString("%1 [%2]").arg(baseTabName).arg(chat.unreadMessagesCount()),
		                             QString("%1\n%2 new message(s)").arg(chatWidget->title()).arg(chat.unreadMessagesCount()));
	else
		setTabTextAndTooltipIfDiffer(i, baseTabName, baseTabName);
}

void TabsManager::updateTabIcon(ChatWidget *chatWidget)
{
	Q_ASSERT(chatWidget);

	const int i = TabDialog->indexOf(chatWidget);
	if (-1 == i)
		return;

	if (chatWidget->chat().unreadMessagesCount() > 0)
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

	ChatWidgetManager::instance()->openChat(ClosedChats.takeFirst(), OpenChatActivation::Activate);
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

#include "moc_tabs.cpp"
