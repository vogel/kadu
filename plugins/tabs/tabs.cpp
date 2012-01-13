/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
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

/*
 * autor
 * Michal Podsiadlik
 * michal at gov.one.pl
 */

#include <QtGui/QApplication>
#include <QtGui/QMenu>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/toolbar.h"
#include "icons/kadu-icon.h"
#include "message/message-manager.h"
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
		ConfigurationUiHandler(parent), NoTabs(false), ForceTabs(false), TargetTabs(-1)
{
	kdebugf();

	setState(StateNotLoaded);

	createDefaultConfiguration();

	connect(ChatWidgetManager::instance(), SIGNAL(handleNewChatWidget(ChatWidget *, bool &)),
			this, SLOT(onNewChat(ChatWidget *, bool &)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)),
			this, SLOT(onDestroyingChat(ChatWidget *)));

	connect(&Timer, SIGNAL(timeout()), this, SLOT(onTimer()));

	TabDialog = new TabWidget(this);
	TabDialog->setProperty("ownWindowIcon", true);
	TabDialog->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(TabDialog, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));
	connect(TabDialog, SIGNAL(contextMenu(QWidget *, const QPoint &)),
			this, SLOT(onContextMenu(QWidget *, const QPoint &)));

	loadWindowGeometry(TabDialog, "Chat", "TabWindowsGeometry", 30, 30, 550, 400);

	makePopupMenu();

	// pozycja tabów
	configurationUpdated();

	OpenInNewTabActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "openInNewTabAction",
		this, SLOT(onNewTab(QAction *, bool)),
		KaduIcon("internet-group-chat"), tr("Chat in New Tab"), false, disableNewTab
	);
	TalkableMenuManager::instance()->addActionDescription(OpenInNewTabActionDescription, TalkableMenuItem::CategoryChat, 200);

	AttachToTabsActionDescription = new ActionDescription(this,
		ActionDescription::TypeChat, "attachToTabsAction",
		this, SLOT(onTabAttach(QAction *, bool)),
		KaduIcon("kadu_icons/tab-detach"), tr("Attach Chat to Tabs"), true
	);
	connect(AttachToTabsActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(attachToTabsActionCreated(Action *)));

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		ensureLoaded(); //loadTabs();

	kdebugf2();
}

TabsManager::~TabsManager()
{
	kdebugf();

	TalkableMenuManager::instance()->removeActionDescription(OpenInNewTabActionDescription);

	disconnect(ChatWidgetManager::instance(), 0, this, 0);

	saveWindowGeometry(TabDialog, "Chat", "TabWindowsGeometry");

	// jesli kadu nie konczy dzialania to znaczy ze modul zostal tylko wyladowany wiec odlaczamy rozmowy z kart
	if (!Core::instance()->isClosing())
		for (int i = TabDialog->count() - 1; i >= 0; i--)
			detachChat(static_cast<ChatWidget *>(TabDialog->widget(i)));
	else
		ensureStored();

	delete TabDialog;
	TabDialog = 0;

	delete Menu;
	Menu = 0;

	kdebugf2();
}

void TabsManager::onNewChat(ChatWidget *chatWidget, bool &handled)
{
	kdebugf();

	if (NoTabs)
	{
		NoTabs = false;
		DetachedChats.append(chatWidget);
		return;
	}

	// jesli chat ma zostac bezwzglednie dodany do kart np w wyniku wyboru w menu
	if (ForceTabs)
	{
		ForceTabs = false;
		handled = true;
		insertTab(chatWidget);
		return;
	}

	if (ConfigDefaultTabs && (ConfigConferencesInTabs || chatWidget->chat().contacts().count() == 1))
	{
		// jesli jest juz otwarte okno z kartami to dodajemy bezwzglednie nowe rozmowy do kart
		if (TabDialog->count() > 0)
		{
			handled = true;
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
			handled = true;
			insertTab(chatWidget);
			NewChats.clear();
		}
		else
			NewChats.append(chatWidget);
	}
	kdebugf2();
}

void TabsManager::onDestroyingChat(ChatWidget *chatWidget)
{
	kdebugf();

	if (TabDialog->indexOf(chatWidget) != -1)
	{
		// zapamietuje wewnetrzne rozmiary chata
		chatWidget->kaduStoreGeometry();

		TabDialog->removeTab(TabDialog->indexOf(chatWidget));
	}

	NewChats.removeAll(chatWidget);
	DetachedChats.removeAll(chatWidget);

	removeChatWidgetFromChatWidgetsWithMessage(chatWidget);

	disconnect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)), TabDialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));
	disconnect(chatWidget, SIGNAL(closed()), this, SLOT(closeChat()));
	disconnect(chatWidget, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
	disconnect(chatWidget, SIGNAL(titleChanged(ChatWidget *, const QString &)), this, SLOT(onTitleChanged(ChatWidget *, const QString &)));
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
		MessageManager::instance()->markAllMessagesAsRead(chat);
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

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat, false);
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
			NoTabs = true;
		// w miejsce recznego dodawania chata do kart automatyczne ;)
		else if (chat.contacts().count() == 1 || ConfigConferencesInTabs)
			ForceTabs = true;

		ChatWidgetManager::instance()->byChat(chat, true);
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

	_activateWindow(TabDialog);

	TargetTabs = -1;

	connect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)),
			TabDialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));
	connect(chatWidget, SIGNAL(closed()), this, SLOT(closeChat()));
	connect(chatWidget, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
	connect(chatWidget, SIGNAL(titleChanged(ChatWidget * , const QString &)),
			this, SLOT(onTitleChanged(ChatWidget *, const QString &)));

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
					MessageManager::instance()->markAllMessagesAsRead(chatWidget->chat());
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
	const Chat &chat = chatWidget->chat();
	delete chatWidget;

	// omg this is bad
	NoTabs = true;
	ChatWidget * const detachedChatWidget = ChatWidgetManager::instance()->byChat(chat, true);
	if (detachedChatWidget)
		detachedChatWidget->activate();
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

		ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat, false);
		if (!chatWidget)
		{
			if (element.attribute("type") == "tab")
				ForceTabs = true;
			else if (element.attribute("type") == "detachedChat")
				NoTabs = true;
			ChatWidgetManager::instance()->byChat(chat, true);
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

	foreach (ChatWidget *chatWidget, ChatWidgetManager::instance()->chats())
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
	connect(mainConfigurationWindow->widget()->widgetById("tabs/DefaultTabs"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("tabs/MinTabs"), SLOT(setEnabled(bool)));
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

	// Sprawdzam czy sa jakies konferencje a jesli sa to ustawiam w nich poprawnie przyciski w zaleznosci
	// czy opcja "Konferencje w kartach" jest wlaczona/wylaczona
	/*
	ChatList chList = ChatManager::instance()->chats();
	for (int i = chList.count()-1; i>=0; i--)
	{
		KaduAction *action = attachToTabsActionDescription->action(chList[i]->getChatEditBox());
		if (!action || tabdialog->indexOf(chList[i])!=-1)
			continue;

		if (action->contacts().count() > 1)
				action->setEnabled(config_conferencesInTabs);

	}
	*/
	TabDialog->configurationUpdated();

	DetachTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-detach").icon());
	CloseTabMenuAction->setIcon(KaduIcon("kadu_icons/tab-close").icon());

	kdebugf2();
}

void TabsManager::openTabWith(QStringList altnicks, int index)
{
	Q_UNUSED(altnicks)
	Q_UNUSED(index)

	/*
	ContactList contacts;
	//foreach(QString altnick, altnicks)
	//	contacts.append(userlist->byAltNick(altnick).toContact());
	ChatWidget *chatWidget=ChatWidgetManager::instance()->findChatWidget(contacts);
	if (chatWidget)
		if(tabdialog->indexOf(chatWidget)!=-1)
		// Jesli chat istnieje i jest dodany do kart, to czynimy go aktywnym
			onOpenChat(chatWidget);
		else
		{
		// Jesli chat istnieje i nie jest w kartach to dodajemy go do kart na pozycji index
			target_tabs=index;
			insertTab(chatWidget);
		}
	else
	{
	// Jeśli chat nie istnieje to go tworzymy z wymuszonym dodaniem go do kart
		force_tabs=true;
		target_tabs=index;
		ChatWidgetManager::instance()->openChat(contacts);
	}
	*/
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

void TabsManager::createDefaultConfiguration()
{
	config_file.addVariable("ShortCuts", "MoveTabLeft", "Ctrl+Alt+Left");
	config_file.addVariable("ShortCuts", "MoveTabRight", "Ctrl+Alt+Right");
	config_file.addVariable("ShortCuts", "SwitchTabLeft", "Alt+Left");
	config_file.addVariable("ShortCuts", "SwitchTabRight", "Alt+Right");
	config_file.addVariable("Chat", "ConferencesInTabs", "true");
	config_file.addVariable("Chat", "TabsBelowChats", "false");
	config_file.addVariable("Chat", "DefaultTabs", "true");
	config_file.addVariable("Chat", "MinTabs", "1");
	config_file.addVariable("Tabs", "CloseButton", "true");
	config_file.addVariable("Tabs", "OpenChatButton", "true");
	config_file.addVariable("Tabs", "OldStyleClosing", "false");
	config_file.addVariable("Tabs", "CloseButtonOnTab", "false");
}
