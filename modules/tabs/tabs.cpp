/*
 * %kadu copyright begin%
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

// TODO 0.6.6 : load,save options
// TODO 0.6.6 : load,save chats - remove old metods
// TODO 0.6.6 : import 0.6.5 chats

/*
 * autor
 * Michal Podsiadlik
 * michal at gov.one.pl
 */

#include <QtGui/QApplication>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/toolbar.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

#include "activate.h"
#include "debug.h"
#include "icons-manager.h"

#include "tabs.h"

extern "C" KADU_EXPORT int tabs_init(bool firstload)
{
	tabs_manager = new TabsManager(firstload);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/tabs.ui"));
	return 0;
}

extern "C" KADU_EXPORT void tabs_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/tabs.ui"));
	delete tabs_manager;
	tabs_manager = 0;
}

void disableNewTab(Action *action)
{
	action->setEnabled(false);
	ContactSet contacts = action->contacts();

	if (!contacts.count())
		return;

	bool config_defaultTabs = config_file.readBoolEntry("Chat", "DefaultTabs");
	action->setEnabled(true);

	if (contacts.count() != 1 && !config_defaultTabs)
		action->setEnabled(false);

	if (config_defaultTabs)
		action->setText(qApp->translate("TabsManager", "Open in new window"));
	else
		action->setText(qApp->translate("TabsManager", "Open in new tab"));

	// TODO 0.6.6 dla siebie samego deaktywujemy opcje w menu, a konfernecje?
	foreach (const Contact &contact, contacts)
	{
		if (Core::instance()->myself() == contact.ownerBuddy())
			return;

		Account account = contact.contactAccount();
		if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->chatService())
			return;
	}

	action->setEnabled(true);

	kdebugf2();
}

TabsManager::TabsManager(bool firstload)
{
	kdebugf();

	setState(StateNotLoaded);

	createDefaultConfiguration();

	connect(ChatWidgetManager::instance(), SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)),
			this, SLOT(onDestroyingChat(ChatWidget *)));

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetOpen(ChatWidget *)),
			this, SLOT(onOpenChat(ChatWidget *)));

	connect(&timer, SIGNAL(timeout()),
			this, SLOT(onTimer()));

	ConfigurationManager::instance()->registerStorableObject(this);

	openInNewTabActionDescription = new ActionDescription(
		0, ActionDescription::TypeUser, "openInNewTabAction",
		this, SLOT(onNewTab(QAction *, bool)),
		"OpenChat", tr("Open in new tab"), false, QString::null, disableNewTab
	);
	BuddiesListViewMenuManager::instance()->insertActionDescription(1, openInNewTabActionDescription);

	attachToTabsActionDescription = new ActionDescription(
		0, ActionDescription::TypeChat, "attachToTabsAction",
		this, SLOT(onTabAttach(QAction *, bool)),
		"TabsDetached", tr("Attach chat to tabs"), true, tr("Detach chat from tabs")
	);
	connect(attachToTabsActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(attachToTabsActionCreated(Action *)));

	if (firstload)
		ChatEditBox::addAction("attachToTabsAction");

	tabdialog = new TabWidget();
	tabdialog->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(tabdialog, SIGNAL(currentChanged(int)),
			this, SLOT(onTabChange(int)));
	connect(tabdialog, SIGNAL(contextMenu(QWidget*, const QPoint&)),
			this, SLOT(onContextMenu(QWidget*, const QPoint&)));
	// TODO: 0.6.6 - implement ContactList ContactManager::byAltNicks(QString)
	//connect(tabdialog, SIGNAL(openTab(QStringList, int)),
	//		this, SLOT(openTabWith(QStringList, int)));

	loadWindowGeometry(tabdialog, "Chat", "TabWindowsGeometry", 30, 30, 400, 400);

	// sygnal wysylany po aktywacji chata. Jest odbierany przez m.in hint-managera (po aktywacji chata znikaja dymki))
	connect(this, SIGNAL(chatWidgetActivated(ChatWidget *)), ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)));
	connect(tabdialog, SIGNAL(chatWidgetActivated(ChatWidget *)), ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)));

	// zrób mi menu :>
	makePopupMenu();

	// pozycja tabów
	configurationUpdated();

	no_tabs = false;
	force_tabs = false;
	autoswith = false;
	target_tabs = -1;

	// przywracamy karty z poprzedniej sesji
	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		ensureLoaded(); //loadTabs();

	kdebugf2();
}

TabsManager::~TabsManager()
{
	kdebugf();

	BuddiesListViewMenuManager::instance()->removeActionDescription(openInNewTabActionDescription);
	delete openInNewTabActionDescription;
	openInNewTabActionDescription = 0;

	delete attachToTabsActionDescription;
	attachToTabsActionDescription = 0;

	disconnect(ChatWidgetManager::instance(), 0, this, 0);

	saveWindowGeometry(tabdialog, "Chat", "TabWindowsGeometry");

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		store(); //saveTabs();

	// jesli kadu nie konczy dzialania to znaczy ze modul zostal tylko wyladowany wiec odlaczamy rozmowy z kart
	
	if (!Core::instance()->isClosing())
	{
		for (int i = tabdialog->count() - 1; i >= 0; i--)
			detachChat(dynamic_cast<ChatWidget *>(tabdialog->widget(i)));
	}
	else // saveTabs()
	{
		store();
	}
	
	ConfigurationManager::instance()->unregisterStorableObject(this);

	delete tabdialog;
	tabdialog = 0;
	kdebugf2();
}

void TabsManager::onNewChat(ChatWidget* chat, bool &handled)
{
	kdebugf();

	if (no_tabs)
	{
		no_tabs = false;
		detachedchats.append(chat);
		return;
	}
	
	// jesli chat ma zostac bezwzglednie dodany do kart np w wyniku wyboru w menu
	if (force_tabs)
	{
		force_tabs = false;
		handled = true;
		insertTab(chat);
		return;
	}

	if (config_defaultTabs && (config_conferencesInTabs || chat->chat().contacts().count() == 1))
	{
		// jesli jest juz otwarte okno z kartami to dodajemy bezwzglednie nowe rozmowy do kart
		if (tabdialog->count() > 0)
		{
			handled = true;
			insertTab(chat);
		}
		else if ((newchats.count() + 1) >= config_minTabs)
		{
			foreach (ChatWidget *ch, newchats)
			{
				// dodajemy karte tylko jesli jej jeszcze nie ma
				if (ch && tabdialog->indexOf(ch)==-1)
					insertTab(ch);
			}
			handled = true;
			insertTab(chat);
			newchats.clear();
		}
		else
			newchats.append(chat);
	}
	kdebugf2();
}

void TabsManager::onDestroyingChat(ChatWidget* chat)
{
	kdebugf();

	if (tabdialog->indexOf(chat) != -1)
	{
		//tabdialog->removePage(chat);
		tabdialog->removeTab(tabdialog->indexOf(chat));
		// zapamietuje wewnetrzne rozmiary chata
		chat->kaduStoreGeometry();
	}

	newchats.removeOne(chat);
	detachedchats.removeOne(chat);
	chatsWithNewMessages.removeOne(chat);
	disconnect(chat->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)), tabdialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));
	disconnect(chat, SIGNAL(messageReceived(Chat)), this, SLOT(onMessageReceived(Chat)));
	disconnect(chat, SIGNAL(closed()), this, SLOT(closeChat()));
	disconnect(chat->chat(), SIGNAL(titleChanged(Chat , const QString &)), this, SLOT(onTitleChanged(Chat , const QString &)));
	kdebugf2();
}

void TabsManager::onTitleChanged(Chat chatChanged, const QString &newTitle)
{
	kdebugf();

	ChatWidget *chat = ChatWidgetManager::instance()->byChat(chatChanged);

	int chatIndex = tabdialog->indexOf(chat);

	if (-1 == chatIndex || 0 == chat)
		return;

	refreshTab(chatIndex, chat);

	if (tabdialog->currentIndex() == chatIndex)
	{
		tabdialog->setWindowTitle(chatChanged.title());
		tabdialog->setWindowIcon(chatChanged.icon());
	}

	kdebugf2();
}

void TabsManager::onTabChange(int index)
{
	if (index < 0)
		return;

	ChatWidget *chat = dynamic_cast<ChatWidget *>(tabdialog->widget(index));

	// czy jest na liscie chatow z nowymi wiadomosciami
	if (chatsWithNewMessages.contains(chat))
		chatsWithNewMessages.removeOne(chat);

	refreshTab(index, chat);

	tabdialog->setWindowTitle(chat->chat().title());
	tabdialog->setWindowIcon(chat->chat().icon());

	emit chatWidgetActivated(chat);
	// ustawiamy focus na pole edycji chata
	chat->edit()->setFocus();
}

void TabsManager::onOpenChat(ChatWidget *chat)
{
	kdebugf();
	if (chat && tabdialog->indexOf(chat)!=-1)
	{
		tabdialog->setWindowState(tabdialog->windowState() & ~Qt::WindowMinimized);
		tabdialog->setCurrentWidget(chat);
		tabdialog->raise();
	}
	else if ((config_autoTabChange && !(chatsWithNewMessages.contains(chat))) ||
		((!_isActiveWindow(tabdialog)) && !(chatsWithNewMessages.contains(chat))) ||
		((chatsWithNewMessages.contains(chat)) && !(config_file.readBoolEntry("Chat","OpenChatOnMessage"))))
			autoswith = true;
	kdebugf2();
}

void TabsManager::onMessageReceived(Chat chat)
{
	kdebugf();

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat, false);
	if (!chatWidget)
		return;

	if (!(chatsWithNewMessages.contains(chatWidget)) && ((tabdialog->currentWidget() != chatWidget) || !_isActiveWindow(tabdialog)))
	{
		chatsWithNewMessages.append(chatWidget);
		if (!timer.isActive())
			timer.start(500);
	}
	// jezelo chat jest aktywny zerujemy licznik nowych wiadomosci
	if (_isActiveWindow(tabdialog) && tabdialog->currentWidget() == chatWidget)
		chatWidget->markAllMessagesRead();
	kdebugf2();
}

void TabsManager::onNewTab(QAction *sender, bool toggled)
{
	kdebugf();

	MainWindow *window = dynamic_cast<MainWindow *>(sender->parent());
	if (!window)
		return;

	ContactSet contacts = window->contacts();
	int contactsCount = contacts.count();

	if (0 == contactsCount)
		return;

	// TODO 0.6.6: check if every contact has the same account
	Account account = (*contacts.begin()).contactAccount();
	if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->chatService())
		return;

	Chat chat = ChatManager::instance()->findChat(contacts);

	// exists - bring to front
	if (chat)
	{
		ChatWidgetManager::instance()->openPendingMsgs(chat, true);
		ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
		if (!chatWidget)
			return;

		if (tabdialog->indexOf(chatWidget) != -1)
		{
			tabdialog->setWindowState(tabdialog->windowState() & ~Qt::WindowMinimized);
			tabdialog->setCurrentWidget(chatWidget);
		}
		_activateWindow(chatWidget);
	}

	// TODO : is it possible?
/*	else
	{
		if (config_defaultTabs)
			no_tabs = true;
		// w miejsce recznego dodawania chata do kart automatyczne ;)
		else if (contactsCount == 1 || config_conferencesInTabs)
			force_tabs = true;
		// but here chat = 0
		ChatWidgetManager::instance()->openPendingMsgs(chat, true);
	}
*/
	kdebugf2();
}

void TabsManager::insertTab(ChatWidget* chat)
{
	kdebugf();

	// jeśli jest otwarty chatwindow przypisany do chat to zostanie on zamknięty
	if (chat->parent())
		chat->parent()->deleteLater();
	else
		chat->kaduRestoreGeometry();

	ContactSet contacts = chat->chat().contacts();

	detachedchats.removeOne(chat);

	foreach (Action *action, attachToTabsActionDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(true);
	}

	// Ustawiam tytul karty w zaleznosci od tego czy mamy do czynienia z rozmowa czy z konferencja
	tabdialog->insertTab(target_tabs, chat, chat->icon(), formatTabName(chat));

	tabdialog->setTabToolTip(target_tabs, chat->chat().title());

	if ((config_autoTabChange && !chatsWithNewMessages.contains(chat)) || autoswith)
		tabdialog->setCurrentWidget(chat);

	tabdialog->setWindowState(tabdialog->windowState() & ~Qt::WindowMinimized);
	_activateWindow(tabdialog);

	autoswith = false;
	target_tabs = -1;

	connect(chat->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)), tabdialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));
	// Podlaczamy sie do nowej wiadomości w chacie, tylko jesli dodany on zostal do kart
	connect(chat, SIGNAL(messageReceived(Chat)), this, SLOT(onMessageReceived(Chat)));
	connect(chat, SIGNAL(closed()), this, SLOT(closeChat()));
	connect(chat->chat(), SIGNAL(titleChanged(Chat , const QString &)), this, SLOT(onTitleChanged(Chat , const QString &)));

	kdebugf2();
}

// uff, troche dziwne to ale dziala tak jak trzeba
// TODO: review this!!!
void TabsManager::onTimer()
{
	kdebugf();
	ChatWidget *chat;
	static bool msg, wasactive = 1;

	bool tabsActive = _isActiveWindow(tabdialog);
	ChatWidget *currentChat = dynamic_cast<ChatWidget *>(tabdialog->currentWidget());
	// sprawdzaj wszystkie okna ktore sa w tabach
	for (int i = tabdialog->count() -1; i >= 0; i--)
	{
		chat = dynamic_cast<ChatWidget *>(tabdialog->widget(i));

		// czy trzeba cos robia ?
		if (chatsWithNewMessages.contains(chat))
		{
			// okno nieaktywne to trzeba cos zrobic
			if (!tabsActive)
			{
				// jesli chat jest na aktywnej karcie - zachowuje sie jak normalne okno
				if (currentChat == chat)
				{	if (msg && config_blinkChatTitle)
						tabdialog->setWindowTitle(QString().fill(' ', (chat->chat().title().length() + 5)));
					else if (!msg)
						if(config_showNewMessagesNum)
							tabdialog->setWindowTitle("[" + QString().setNum(chat->newMessagesCount()) + "] " + chat->chat().title());
						else
							tabdialog->setWindowTitle(chat->chat().title());
				}
				// jesli nie w zaleznosci od konfiguracji wystepuje "miganie" lub nie
				else if (config_blinkChatTitle && !msg)
					tabdialog->setWindowTitle(tr("NEW MESSAGE(S)"));
				else
					tabdialog->setWindowTitle(chat->chat().title());
			}

			// tab aktualnie nieaktywny to ustaw ikonke
			if (currentChat != chat)
			{
				if (msg)
					tabdialog->setTabIcon(i, IconsManager::instance()->loadIcon("Message"));
				else
					tabdialog->setTabIcon(i, chat->icon());
			}
			else if (currentChat == chat && tabsActive)
				// wywal go z listy chatow z nowymi wiadomosciami
				chatsWithNewMessages.removeOne(chat);

			if (tabsActive)
			{
				if (currentChat == chat)
				{
					// zeruje licznik nowch wiadomosci w chat
					chat->markAllMessagesRead();
					// a tutaj przywroc tytul�
					tabdialog->setWindowTitle(chat->chat().title());
				}
				else if (chatsWithNewMessages.count() == 1 && !wasactive && config_autoTabChange)
					tabdialog->setCurrentWidget(chat);
			}
		}
	}

	if (chatsWithNewMessages.size()==0)
		timer.stop();

	wasactive = tabsActive;
	msg = !msg;
	kdebugf2();
}

void TabsManager::onTabAttach(QAction *sender, bool toggled)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	if (!toggled)
		detachChat(chatWidget);
	else
	{
		if (chatEditBox->contacts().count()!=1 && !config_conferencesInTabs)
			return;
		newchats.clear();
		insertTab(chatWidget);
	}
}

void TabsManager::onContextMenu(QWidget *w, const QPoint &pos)
{
	kdebugf();
	selectedchat = dynamic_cast<ChatWidget *>(w);
	menu->popup(pos);
	kdebugf2();
}

void TabsManager::makePopupMenu()
{
	kdebugf();

	menu = new QMenu();
	//menu->setCheckable(true);
	menu->addAction(IconsManager::instance()->loadIcon("TabsDetached"), tr("Detach"), this, SLOT(onMenuActionDetach()));
	menu->addAction(tr("Detach all"), this, SLOT(onMenuActionDetachAll()));
	menu->addSeparator();
	menu->addAction(IconsManager::instance()->loadIcon("TabsClose"), tr("Close"), this, SLOT(onMenuActionClose()));
	menu->addAction(tr("Close all"), this, SLOT(onMenuActionCloseAll()));

	kdebugf2();
}

void TabsManager::onMenuActionDetach()
{
	detachChat(selectedchat);
}

void TabsManager::onMenuActionDetachAll()
{
	for (int i = tabdialog->count()-1; i >= 0; --i)
		detachChat(dynamic_cast<ChatWidget *>(tabdialog->widget(i)));
}

void TabsManager::onMenuActionClose()
{
	delete selectedchat;
}

void TabsManager::onMenuActionCloseAll()
{
	for (int i = tabdialog->count() - 1; i >= 0; --i)
		delete tabdialog->widget(i);
}

void TabsManager::attachToTabsActionCreated(Action *action)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	ContactSet contacts = action->contacts();

	if (contacts.count() != 1 && !config_conferencesInTabs && tabdialog->indexOf(chatWidget) == -1)
		action->setEnabled(false);

	action->setChecked(tabdialog->indexOf(chatWidget) != -1);
}

bool TabsManager::detachChat(ChatWidget* chat)
{
	kdebugf();
	if (tabdialog->indexOf(chat) == -1)
		return false;
	Chat oldchat = chat->chat();
	delete chat;

	no_tabs = true;
	ChatWidgetManager::instance()->openPendingMsgs(oldchat, true);
	return true;
	kdebugf2();
}

void TabsManager::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	XmlConfigFile *storageFile = storage()->storage();
	QDomElement point = storage()->point();

	QList<QDomElement> nodes = storageFile->getNodes(point, "Tab");
	foreach (QDomElement element, nodes)
	{
		QUuid chatId(element.attribute("chat"));

		if (chatId.isNull())
			continue;

		Chat chat = ChatManager::instance()->byUuid(chatId);
		if (!chat)
			continue;

		ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
		if (!chatWidget)
			continue;

		if (element.attribute("type") == "tab")
			insertTab(chatWidget);
		else if (element.attribute("type") == "detachedChat")
			detachedchats.append(chatWidget);
	}
}

void TabsManager::store()
{
  	if (!isValidStorage())
		return;

	XmlConfigFile *storageFile = storage()->storage();
	QDomElement point = storage()->point();

	storageFile->removeChildren(point);

	foreach (ChatWidget * chatWidget, ChatWidgetManager::instance()->chats().values())
	{
		if (!chatWidget)
			continue;

		Chat  chat = chatWidget->chat();

		if (!chat)
			continue;

		if ((tabdialog->indexOf(chatWidget) == -1) && (detachedchats.indexOf(chatWidget) == -1))
			continue;

		QDomElement window_elem = storageFile->createElement(point, "Tab");

		window_elem.setAttribute("chat", chat.uuid() );
		if (tabdialog->indexOf(chatWidget) != -1)
			window_elem.setAttribute("type", "tab");
		else if (detachedchats.indexOf(chatWidget) != -1)
			window_elem.setAttribute("type", "detachedChat");
	}
}

void TabsManager::loadTabs()
{
	kdebugf();
/*
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->findElement(root_elem, "TabsChats");
	if (!chats_elem.isNull())
	{
		ChatWidget* chat;
		for (QDomNode win = chats_elem.firstChild(); !win.isNull(); win = win.nextSibling())
		{
			const QDomElement &window_elem = win.toElement();
			if (window_elem.isNull() || window_elem.tagName() != "Tab")
				continue;
			QString account_id = window_elem.attribute("account");
			ContactList users;
			for (QDomNode contact = window_elem.firstChild(); !contact.isNull(); contact = contact.nextSibling())
			{
				const QDomElement &contact_elem = contact.toElement();
				if (contact_elem.isNull() || contact_elem.tagName() != "Contact")
					continue;
				QString contact_uuid = contact_elem.attribute("id");
				users.append(ContactManager::instance()->getContactByUuid(contact_uuid));
			}
			chat=ChatManager::instance()->findChatWidget(users);
			// jeśli nie istnieje to tworzymy
			if (!chat)
			{
				if (window_elem.attribute("type")=="tab")
					force_tabs=true;
				else if (window_elem.attribute("type")=="detachedChat")
					no_tabs=true;
				ChatManager::instance()->openChatWidget(AccountManager::instance()->account(account_id),users, false);
			}
			else if (window_elem.attribute("type")=="tab")
				insertTab(chat);
			if (window_elem.attribute("type")=="detachedChat")
				detachedchats.append(chat);
		}
		// usuwamy z konfiguracji przywrocone rozmowy
		xml_config_file->removeChildren(chats_elem);
	}
*/
	kdebugf2();
}

void TabsManager::saveTabs()
{
	kdebugf();
/*
	ChatWidget* chat;
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->accessElement(root_elem, "TabsChats");
	xml_config_file->removeChildren(chats_elem);

	ChatList chList = ChatManager::instance()->chats();
	for (uint i = 0; i < chList.count(); i++)
	{
		chat=chList[i];
		if (!(tabdialog->indexOf(chList[i])!=-1) && !(detachedchats.findIndex(chList[i])!=-1))
			continue;
		QDomElement window_elem = xml_config_file->createElement(chats_elem, "Tab");

		Account *account = chat->account();
		window_elem.setAttribute("account", account->uuid() );
		if (tabdialog->indexOf(chList[i])!=-1)
			window_elem.setAttribute("type", "tab");
		else if (detachedchats.findIndex(chList[i])!=-1)
			window_elem.setAttribute("type", "detachedChat");

		//QDomElement contactListNode = configurationStorage->getNode(pendingMessageNode, "ContactList", XmlConfigFile::ModeCreate);
		//QDomElement contact_list_elem = xml_config_file->createElement(window_elem, "ContactList");
		// TODO: 0.6.6 extract class - ContactListHelper
		foreach(Contact c, chat->contacts())
		{
			//contact_list_elem->createTextNode(contact_list_elem, "Contact", c.uuid());
			QDomElement user_elem = xml_config_file->createElement(window_elem, "Contact");
			user_elem.setAttribute("id", c.uuid());
		}
	}
*/
	kdebugf2();
}

void TabsManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("tabs/DefaultTabs"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("tabs/MinTabs"), SLOT(setEnabled(bool)));
}

void TabsManager::configurationUpdated()
{
	kdebugf();
	config_conferencesInTabs = config_file.readBoolEntry("Chat", "ConferencesInTabs");
	config_tabsBelowChats = config_file.readBoolEntry("Chat", "TabsBelowChats");
	config_autoTabChange = config_file.readBoolEntry("Chat", "AutoTabChange");
	config_defaultTabs = config_file.readBoolEntry("Chat", "DefaultTabs");
	config_minTabs = config_file.readUnsignedNumEntry("Chat", "MinTabs");
	config_blinkChatTitle = config_file.readBoolEntry("Chat", "BlinkChatTitle");
	config_showNewMessagesNum = config_file.readBoolEntry("Chat", "NewMessagesInChatTitle");

	tabdialog->setTabPosition(config_tabsBelowChats ? QTabWidget::South : QTabWidget::North);

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
	tabdialog->configurationUpdated();
	// w zaleznosci od opcji w konfiguracji rezerwujemy miejsce na przycisk zamkniecia chata na karcie lub je usuwamy
	config_closeButtonOnTab = config_file.readBoolEntry("Tabs", "CloseButtonOnTab");
	repaintTabs();

	//uaktualniamy ikonki w menu kontekstowym pod PPM na karcie
	// TODO : to remove ?
	//menu->changeItem(0, IconsManager::instance()->loadIcon("TabsDetached"), tr("Detach"));
	//menu->changeItem(2, IconsManager::instance()->loadIcon("TabsClose"), tr("Close"));

	kdebugf2();
}

void TabsManager::openTabWith(QStringList altnicks, int index)
{
	/*
	ContactList contacts;
	//foreach(QString altnick, altnicks)
	//	contacts.append(userlist->byAltNick(altnick).toContact());
	ChatWidget* chat=ChatWidgetManager::instance()->findChatWidget(contacts);
	if (chat)
		if(tabdialog->indexOf(chat)!=-1)
		// Jesli chat istnieje i jest dodany do kart, to czynimy go aktywnym
			onOpenChat(chat);
		else
		{
		// Jesli chat istnieje i nie jest w kartach to dodajemy go do kart na pozycji index
			target_tabs=index;
			insertTab(chat);
		}
	else
	{
	// Jeśli chat nie istnieje to go tworzymy z wymuszonym dodaniem go do kart
		force_tabs=true;
		target_tabs=index;
		ChatWidgetManager::instance()->openPendingMsgs(contacts, true);
	}
	*/
}

void TabsManager::repaintTabs()
{
	if (!tabdialog->count())
		return;

	ChatWidget *chat;

	for (int i = tabdialog->count() -1 ; i >= 0; i--)
	{
		chat = dynamic_cast<ChatWidget *>(tabdialog->widget(i));

		refreshTab(i, chat);
	}

	//uaktualnienie ikonki w oknie tabs
	tabdialog->setWindowIcon(dynamic_cast<ChatWidget *>(tabdialog->currentWidget())->icon());
}

QString TabsManager::formatTabName(ChatWidget * chatWidget)
{
	int contactsCount = chatWidget->chat().contacts().count();

	QString TabName;

	if (contactsCount > 1)
		TabName = tr("Conference [%1]").arg(contactsCount);
	else
		TabName = chatWidget->chat().name();

	return TabName;
}

void TabsManager::refreshTab(int tabIndex, ChatWidget * chatWidget)
{
	if (0 == chatWidget)
		return;

	Chat chat = chatWidget->chat();

	if (0 == chat)
		return;

	// uaktualnienie podp.
	tabdialog->setTabToolTip(tabIndex, chat.title());

	//uaktualnienie ikonki
	tabdialog->setTabIcon(tabIndex, chat.icon());

	// uaktualnienie nazwy
	tabdialog->setTabText(tabIndex, formatTabName(chatWidget));
}

void TabsManager::closeChat()
{
	QObject *chat = sender();
	if (chat)
		chat->deleteLater();
}

void TabsManager::createDefaultConfiguration()
{
	config_file.addVariable("ShortCuts", "MoveTabLeft", "Ctrl+Shift+Left");
	config_file.addVariable("ShortCuts", "MoveTabRight", "Ctrl+Shift+Right");
	config_file.addVariable("ShortCuts", "SwitchTabLeft", "Shift+Left");
	config_file.addVariable("ShortCuts", "SwitchTabRight", "Shift+Right");
	config_file.addVariable("Chat", "ConferencesInTabs", "true");
	config_file.addVariable("Chat", "TabsBelowChats", "false");
	config_file.addVariable("Chat", "AutoTabChange", "false");
	config_file.addVariable("Chat", "DefaultTabs", "true");
	config_file.addVariable("Chat", "MinTabs", "1");
	config_file.addVariable("Tabs", "CloseButton", "true");
	config_file.addVariable("Tabs", "OpenChatButton", "true");
	config_file.addVariable("Tabs", "OldStyleClosing", "false");
	config_file.addVariable("Tabs", "CloseButtonOnTab", "false");
}

TabsManager *tabs_manager;
