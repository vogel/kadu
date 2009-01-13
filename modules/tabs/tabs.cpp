/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * autor
 * Michal Podsiadlik
 * michal at gov.one.pl
 */
#include <QtGui/QApplication>
#include <QtGui/QMenu>

#include "action.h"
#include "accounts/account.h"
#include "accounts/account_data.h"
#include "accounts/account_manager.h"
#include "contacts/contact.h"
#include "contacts/contact-list.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
#include "chat_edit_box.h"
#include "debug.h"
#include "icons_manager.h"
#include "toolbar.h"
#include "userbox.h"
#include "xml_config_file.h"

#include "tabs.h"

extern "C" KADU_EXPORT int tabs_init(bool firstload)
{
	tabs_manager=new TabsManager(firstload);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/tabs.ui"), tabs_manager);
	return 0;
}

extern "C" KADU_EXPORT void tabs_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/tabs.ui"), tabs_manager);
	delete tabs_manager;
	tabs_manager=0;
}

void disableNewTab(KaduAction *action)
{
	ContactList contacts = action->contacts();

	bool config_defaultTabs = config_file.readBoolEntry("Chat", "DefaultTabs");

	if (contacts.count() != 1 && !config_defaultTabs && !config_file.readBoolEntry("Chat", "DefaultTabs"))
		action->setEnabled(false);

	if (config_defaultTabs)
		action->setText(qApp->translate("TabsManager", "Open in new window"));
	else
		action->setText(qApp->translate("TabsManager", "Open in new tab"));

	// TODO 0.6.6 dla siebie samego deaktywujemy opcję w menu
	Account *account = AccountManager::instance()->defaultAccount();
	UserListElements users = UserListElements::fromContactList(contacts, account);
	QString myGGUIN = QString::number(config_file.readNumEntry("General", "UIN"));
	foreach(UserListElement user, users)
		if (!user.usesProtocol("Gadu") || user.ID("Gadu") == myGGUIN)
			action->setEnabled(false);
}

TabsManager::TabsManager(bool firstload) : QObject()
{
	kdebugf();

	connect(chat_manager, SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)),
			this, SLOT(onDestroyingChat(ChatWidget *)));

	connect(chat_manager, SIGNAL(chatWidgetOpen(ChatWidget *)),
			this, SLOT(onOpenChat(ChatWidget *)));

	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
			this, SLOT(onStatusChanged(UserListElement/*, QString, const UserStatus &, bool, bool*/)));

	connect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));

	connect(&timer, SIGNAL(timeout()),
			this, SLOT(onTimer()));

	// przeniesienie starej konfiguracji skrotow z Chat do ShortCuts
	// TODO: pozbyć się tego w kadu 0.7
	config_file.addVariable("ShortCuts", "MoveTabLeft", config_file.readEntry("Chat", "MoveTabLeft"));
	config_file.addVariable("ShortCuts", "MoveTabRight", config_file.readEntry("Chat", "MoveTabRight"));
	config_file.addVariable("ShortCuts", "SwitchTabLeft", config_file.readEntry("Chat", "SwitchTabLeft"));
	config_file.addVariable("ShortCuts", "SwitchTabRight", config_file.readEntry("Chat", "SwitchTabRight"));

	// ustawienie domyślnych wartości opcji konfiguracyjnych
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

	openInNewTabActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "openInNewTabAction",
		this, SLOT(onNewTab(QAction *, bool)),
		"OpenChat", tr("Open in new tab"), false, QString::null, disableNewTab
	);
	UserBox::insertActionDescription(1, openInNewTabActionDescription);

	attachToTabsActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "attachToTabsAction",
		this, SLOT(onTabAttach(QAction *, bool)),
		"TabsDetached", tr("Attach chat to tabs"), true, tr("Detach chat from tabs")
	);
	connect(attachToTabsActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(attachToTabsActionCreated(KaduAction *)));

	if(firstload)
		ChatEditBox::addAction("attachToTabsAction");

	tabdialog = new TabWidget();
	tabdialog->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(tabdialog, SIGNAL(currentChanged(int)),
			this, SLOT(onTabChange(int)));
	connect(tabdialog, SIGNAL(contextMenu(QWidget*, const QPoint&)),
			this, SLOT(onContextMenu(QWidget*, const QPoint&)));
	connect(tabdialog, SIGNAL(openTab(QStringList, int)),
			this, SLOT(openTabWith(QStringList, int)));

	loadWindowGeometry(tabdialog, "Chat", "TabWindowsGeometry", 30, 30, 400, 400);

	// sygnał wysyłany po aktywacji chata. Jest odbierany przez m.in hint-managera (po aktywacji chata znikają dymki))
	connect(this, SIGNAL(chatWidgetActivated(ChatWidget *)), chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)));
	connect(tabdialog, SIGNAL(chatWidgetActivated(ChatWidget *)), chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)));

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
		loadTabs();

	// sprawdzanie czy przed załadowaniem modułu zostały utworzone jakieś chaty i dodanie ich do listy
	if (config_defaultTabs)
	{
		ChatList chList = chat_manager->chats();
		for (uint i = 0; i < chList.count(); i++)
		{
			UserListElements uins (UserListElements::fromContactList(chList[i]->contacts(), AccountManager::instance()->defaultAccount()));
			if ((uins.count() > 1 && !config_conferencesInTabs) || tabdialog->indexOf(chList[i])!=-1 || detachedchats.findIndex(chList[i])!=-1)
				continue;
			bool handled;
			onNewChat(chList[i],handled);
		}
	}
	kdebugf2();
}

TabsManager::~TabsManager()
{
	kdebugf();

	UserBox::removeActionDescription(openInNewTabActionDescription);
	delete openInNewTabActionDescription;
	openInNewTabActionDescription = 0;

	delete attachToTabsActionDescription;
	attachToTabsActionDescription = 0;

	disconnect(chat_manager, 0, this, 0);

	saveWindowGeometry(tabdialog, "Chat", "TabWindowsGeometry");

	// jesli kadu nie konczy dzialania to znaczy ze modul zostal tylko wyladowany wiec odlaczamy rozmowy z kart
	if (!Kadu::closing())
	{
		for(int i = tabdialog->count() - 1; i >= 0; i--)
			detachChat(dynamic_cast<ChatWidget *>(tabdialog->page(i)));
	}
	else if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		saveTabs();

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
	// jeśli chat ma zostać bezwzględnie dodany do kart np w wyniku wyboru w menu
	if (force_tabs)
	{
		force_tabs = false;
		handled = true;
		insertTab(chat);
		return;
	}

	if (config_defaultTabs && (config_conferencesInTabs || chat->contacts().count() == 1))
	{
		// jesli jest juz otwarte okno z kartami to dodajemy bezwzglednie nowe rozmowy do kart
		if (tabdialog->count() > 0)
		{
			handled = true;
			insertTab(chat);
		}
		else if ((newchats.count() + 1) >= config_minTabs)
		{
			foreach(ChatWidget *ch, newchats)
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
		// zapami�tuje wewn�trzne rozmiary chata
		chat->kaduStoreGeometry();
	}
	if (tabdialog->count() == 0)
		tabdialog->hide();
	newchats.remove(chat);
	detachedchats.remove(chat);
	chatsWithNewMessages.remove(chat);
	disconnect(chat->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)), tabdialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));
	disconnect(chat, SIGNAL(messageReceived(ChatWidget *)), this, SLOT(onMessageReceived(ChatWidget *)));
	disconnect(chat, SIGNAL(closed()), this, SLOT(closeChat()));
	kdebugf2();
}

/*
 * XXX: jak bedzie obsluga wielu protokolow to to bedzie trzeba lekko przerobic
 */
void TabsManager::onStatusChanged(UserListElement ule)
{
	kdebugf();
	ChatWidget* chat=chat_manager->findChatWidget(ule.toContact());

	if (tabdialog->indexOf(chat)!=-1)
	{
		// zdarzenie nie uaktualniala opisu - trzeba o to zadbac
		chat->refreshTitle();

		tabdialog->setTabToolTip(chat, chat->caption());
		if (tabdialog->currentPage()==chat)
		{
			tabdialog->setCaption(chat->caption());
			tabdialog->setIcon(chat->icon());
		}
		// w zależności od opcji w konfiguracji odpowiednio uaktualniamy tytuł karty
		if(config_closeButtonOnTab)
			tabdialog->changeTab(chat, chat->icon(), ule.altNick()+"  ");
		else
			tabdialog->changeTab(chat, chat->icon(), ule.altNick());

	}
	kdebugf2();
}

void TabsManager::userDataChanged(UserListElement ule, QString name, QVariant /*oldValue*/,QVariant /*currentValue*/ , bool /*massively*/, bool /*last*/)
{
	kdebugf();
	if (name != "AltNick")
		return;
	// jeśli zmienił się nick osoby z którą mamy rozmowę w kartach to uaktualniamy tytuł karty
	onStatusChanged(ule);

	kdebugf2();
}

void TabsManager::onTabChange(int index)
{
	if(index<0)
		return;

	ChatWidget* chat = dynamic_cast<ChatWidget *>(tabdialog->widget(index));

	// czy jest na liście uin-ów z nowymi wiadomościami
	if (chatsWithNewMessages.contains(chat))
		chatsWithNewMessages.remove(chat);

	tabdialog->setIcon(chat->icon());
	tabdialog->setTabToolTip(chat, chat->caption());
	tabdialog->setCaption(chat->caption());
	tabdialog->changeTab(chat, chat->icon(), tabdialog->tabLabel(tabdialog->currentPage()));
	emit chatWidgetActivated(chat);
	// ustawiamy focus na pole edycji chata
	chat->edit()->setFocus();
}

void TabsManager::onOpenChat(ChatWidget *chat)
{
	kdebugf();
	if (chat && tabdialog->indexOf(chat)!=-1)
	{
		tabdialog->setWindowState(tabdialog->windowState() & Qt::WindowMinimized);
		tabdialog->setCurrentPage(tabdialog->indexOf(chat));
		tabdialog->raise();
	}
	else if ((config_autoTabChange && !(chatsWithNewMessages.contains(chat))) ||
		((!tabdialog->isActiveWindow()) && !(chatsWithNewMessages.contains(chat))) ||
		((chatsWithNewMessages.contains(chat)) && !(config_file.readBoolEntry("Chat","OpenChatOnMessage"))))
			autoswith=true;
	kdebugf2();
}

void TabsManager::onMessageReceived(ChatWidget *chat)
{
	kdebugf();
	if (!(chatsWithNewMessages.contains(chat)) && ((tabdialog->currentPage() != chat) || !tabdialog->isActiveWindow()))
	{
		chatsWithNewMessages.append(chat);
		if (!timer.isActive())
			timer.start(500);
	}
	// jeśli chat jest aktywny zerujemy licznik nowych wiadomości
	if (tabdialog->isActiveWindow() && tabdialog->currentPage() == chat)
		chat->markAllMessagesRead();
	kdebugf2();
}

void TabsManager::onNewTab(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	if (contacts.count() == 0)
		return;

	ChatWidget* chat=chat_manager->findChatWidget(contacts);

	// istnieje = przywracamy na pierwszy plan
	if (chat)
	{
		if(tabdialog->indexOf(chat) != -1)
		{
			tabdialog->setWindowState(tabdialog->windowState() & ~Qt::WindowMinimized);
			tabdialog->setCurrentPage(tabdialog->indexOf(chat));
		}
		chat->raise();
		chat->setActiveWindow();
	}
	else
	{
		if (config_defaultTabs)
			no_tabs = true;
		// w miejsce ręcznego dodawaia chata do kart automatyczne ;)
		else if (contacts.count() == 1 || config_conferencesInTabs)
			force_tabs = true;

		chat_manager->openPendingMsgs(contacts, true);
	}

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

	ContactList contacts = chat->contacts();
	UserListElements ules(UserListElements::fromContactList(contacts, AccountManager::instance()->defaultAccount()));

	detachedchats.remove(chat);

	foreach(KaduAction *action, attachToTabsActionDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(true);
	}

	// Ustawiam tytul karty w zaleznosci od tego czy mamy do czynienia z rozmowa czy z konferencja
	if (ules.count()>1)
		// Ustawiam tytul karty w zaleznosci od tego czy przycisk zamknięcia na kartach ma być pokazany
		if(config_closeButtonOnTab)
			tabdialog->insertTab(chat, chat->icon(), tr("Conference [%1]").arg(ules.count())+"  ", target_tabs);
		else
			tabdialog->insertTab(chat, chat->icon(), tr("Conference [%1]").arg(ules.count()), target_tabs);
	else
		// Ustawiam tytul karty w zaleznosci od tego czy przycisk zamknięcia na kartach ma być pokazany
		if(config_closeButtonOnTab)
			tabdialog->insertTab(chat, chat->icon(), ules[0].altNick()+"  ", target_tabs);
		else
			tabdialog->insertTab(chat, chat->icon(), ules[0].altNick(), target_tabs);

	if ((config_autoTabChange && !chatsWithNewMessages.contains(chat)) || autoswith)
		tabdialog->setCurrentPage(tabdialog->indexOf(chat));
	tabdialog->setWindowState(tabdialog->windowState() & Qt::WindowMinimized);
	tabdialog->show();
	tabdialog->raise();
	tabdialog->setActiveWindow();

	autoswith=false;
	target_tabs=-1;

	connect(chat->edit(), SIGNAL(keyPressed(QKeyEvent*, CustomInput*, bool&)), tabdialog, SLOT(chatKeyPressed(QKeyEvent*, CustomInput*, bool&)));
	// Podłączamy sie do nowej wiadomości w chacie, tylko jeśli dodany on został do kart
	connect(chat, SIGNAL(messageReceived(ChatWidget *)), this, SLOT(onMessageReceived(ChatWidget *)));
	connect(chat, SIGNAL(closed()), this, SLOT(closeChat()));

	kdebugf2();
}

// uff, troche dziwne to ale działa tak jak trzeba
void TabsManager::onTimer()
{
	kdebugf();
	ChatWidget* chat;
	static bool msg, wasactive=1;

	// sprawdzaj wszystkie okna które są w tabach
	for(int i = tabdialog->count()-1; i>=0; i--)
	{
		chat = dynamic_cast<ChatWidget *>(tabdialog->page(i));

		// czy trzeba coś robić ?
		if (chatsWithNewMessages.contains(chat))
		{
			// okno nieaktywne to trzeba coś zrobić
			if (!tabdialog->isActiveWindow())
			{
				// jeśli chat jest na aktywneh karcie - zachowuje się jak normalne okno
				if (tabdialog->currentPage() == chat)
				{	if(msg && config_blinkChatTitle)
						tabdialog->setCaption(QString().fill(' ', (chat->caption().length() + 5)));
					else if (!msg)
						if(config_showNewMessagesNum)
							tabdialog->setCaption("[" + QString().setNum(chat->newMessagesCount()) + "] " + chat->caption());
						else
							tabdialog->setCaption(chat->caption());
				}
				// jeśli nie w zależności od konfiguracji występuje "miganie" lub nie
				else if (config_blinkChatTitle && !msg)
					tabdialog->setCaption(tr("NEW MESSAGE(S)"));
				else
					tabdialog->setCaption(chat->caption());
			}

			// tab aktualnie nieaktywny to ustaw ikonke
			if (tabdialog->currentPage() != chat)
			{
				if (msg)
					tabdialog->setTabIconSet(chat, icons_manager->loadIcon("Message"));
				else
					tabdialog->setTabIconSet(chat, chat->icon());
			}
			else if (tabdialog->currentPage()==chat && tabdialog->isActiveWindow())
				// wywal go z listy uin-ów z nowymi wiadomościami
				chatsWithNewMessages.remove(chat);

			if (tabdialog->isActiveWindow())
			{
				if (tabdialog->currentPage()==chat)
				{
					// zeruje licznik nowch wiadomosci w chat
					chat->markAllMessagesRead();
					// a tutaj przywróć tytuł
					tabdialog->setCaption(chat->caption());
				}
				else if (chatsWithNewMessages.count() == 1 && !wasactive && config_autoTabChange)
					tabdialog->setCurrentPage(tabdialog->indexOf(chat));
			}
		}
	}

	if (chatsWithNewMessages.size()==0)
		timer.stop();

	wasactive = tabdialog->isActiveWindow();
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

void TabsManager::onContextMenu(QWidget* w, const QPoint& pos)
{
	kdebugf();
	///to już powinno działać
	selectedchat = dynamic_cast<ChatWidget *>(w);
	menu->popup(pos);
	kdebugf2();
}

void TabsManager::makePopupMenu()
{
	kdebugf();

	menu=new QMenu();
	menu->setCheckable(true);
	menu->insertItem(icons_manager->loadIcon("TabsDetached"), tr("Detach"), 0);
	menu->insertItem(tr("Detach all"), 1);
	menu->insertSeparator();
	menu->insertItem(icons_manager->loadIcon("TabsClose"), tr("Close"), 2);
	menu->insertItem(tr("Close all"), 3);

	connect(menu, SIGNAL(activated(int)), this, SLOT(onMenu(int)));
	kdebugf2();
}

void TabsManager::onMenu(int id)
{
	switch(id)
	{
		case 0:
			detachChat(selectedchat);
			break;
		case 1:
			for(int i=tabdialog->count()-1; i>=0; i--)
				detachChat(dynamic_cast<ChatWidget *>(tabdialog->page(i)));
			break;
		case 2:
			delete selectedchat;
			break;
		case 3:
			for(int i=tabdialog->count()-1; i>=0; i--)
				delete tabdialog->page(i);
			break;
	}
}

void TabsManager::attachToTabsActionCreated(KaduAction *action)
{
	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;
	ContactList contacts = action->contacts();

	if (contacts.count() != 1 && !config_conferencesInTabs && tabdialog->indexOf(chatWidget) == -1)
		action->setEnabled(false);

	action->setChecked(tabdialog->indexOf(chatWidget) != -1);
}


bool TabsManager::detachChat(ChatWidget* chat)
{
	kdebugf();
	if (tabdialog->indexOf(chat) == -1)
		return false;
	ContactList contacts=chat->contacts();
	delete chat;

	no_tabs = true;
	chat_manager->openPendingMsgs(contacts, true);
	return true;
	kdebugf2();
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
			chat=chat_manager->findChatWidget(users);
			// jeśli nie istnieje to tworzymy
			if (!chat)
			{
				if (window_elem.attribute("type")=="tab")
					force_tabs=true;
				else if (window_elem.attribute("type")=="detachedChat")
					no_tabs=true;
				chat_manager->openChatWidget(AccountManager::instance()->account(account_id),users, false);
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

	ChatList chList = chat_manager->chats();
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
	connect(mainConfigurationWindow->widgetById("tabs/DefaultTabs"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("tabs/MinTabs"), SLOT(setEnabled(bool)));
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

	if (!config_tabsBelowChats)
		tabdialog->setTabPosition(QTabWidget::Top);
	else
		tabdialog->setTabPosition(QTabWidget::Bottom);
	// Sprawdzam czy są jakieś konferencje a jeśli są to ustawiam w nich poprawnie przyciski w zaleznosci
	// czy opcja "Konferencje w kartach" jest włączona/wyłączona
	ChatList chList = chat_manager->chats();
	for (int i = chList.count()-1; i>=0; i--)
	{
		KaduAction *action = attachToTabsActionDescription->action(chList[i]->getChatEditBox());
		if (!action || tabdialog->indexOf(chList[i])!=-1)
			continue;

		if (action->contacts().count() > 1)
				action->setEnabled(config_conferencesInTabs);

	}
	tabdialog->configurationUpdated();
	// w zaleśnośći od opcji w konfiguracji rezerwujemy miejsce na przycisk zamknięcia chata na karcie lub je usuwamy
	config_closeButtonOnTab = config_file.readBoolEntry("Tabs", "CloseButtonOnTab");
	repaintTabs();

	//uaktualniamy ikonki w menu kontekstowym pod PPM na karcie
	menu->changeItem(0, icons_manager->loadIcon("TabsDetached"), tr("Detach"));
	menu->changeItem(2, icons_manager->loadIcon("TabsClose"), tr("Close"));

	kdebugf2();
}

void TabsManager::openTabWith(QStringList altnicks, int index)
{
	ContactList contacts;
	foreach(QString altnick, altnicks)
		contacts.append(userlist->byAltNick(altnick).toContact());
	ChatWidget* chat=chat_manager->findChatWidget(contacts);
	if (chat)
		if(tabdialog->indexOf(chat)!=-1)
		// Jeśli chat istnieje i jest dodany do kart, to czynimy go aktywnym
			onOpenChat(chat);
		else
		{
		// Jeśli chat istnieje i nie jest w kartach to dodajemy go do kart na pozycji index
			target_tabs=index;
			insertTab(chat);
		}
	else
	{
	// Jeśli chat nie istnieje to go tworzymy z wymuszonym dodaniem go do kart
		force_tabs=true;
		target_tabs=index;
		chat_manager->openPendingMsgs(contacts, true);
	}
}

void TabsManager::repaintTabs()
{
	if(!tabdialog->count())
		return;
	ChatWidget *chat;
	// jeśli przycisk zamknięcia na kartach ma być pokazany
	if(config_closeButtonOnTab)
		// do tytułów wszystkich kart dodaj2my 2 spacje jako miejsce dla przycisku zamknięcia
		for(int i = tabdialog->count()-1; i>=0; i--)
		{
			chat = dynamic_cast<ChatWidget *>(tabdialog->page(i));
			ContactList contacts = chat->contacts();
			//uaktualnienie ikonki
			chat->refreshTitle();

			if (contacts.count()>1)
				tabdialog->changeTab(chat, chat->icon(), tr("Conference [%1]").arg(contacts.count()) +"  ");
			else
				tabdialog->changeTab(chat, chat->icon(), contacts[0].display() +"  ");
		}
	else
		// jeśli nie przywracamy standardowe tytuły w kartach
		for(int i = tabdialog->count()-1; i>=0; i--)
		{
			chat = dynamic_cast<ChatWidget *>(tabdialog->page(i));
			ContactList contacts = chat->contacts();
			//uaktualnienie ikonki
			chat->refreshTitle();

			if (contacts.count()>1)
				tabdialog->changeTab(chat, chat->icon(), tr("Conference [%1]").arg(contacts.count()));
			else
				tabdialog->changeTab(chat, chat->icon(), contacts[0].display());
		}
	//uaktualnienie ikonki w oknie tabs
	tabdialog->setIcon(dynamic_cast<ChatWidget *>(tabdialog->currentPage())->icon());
}

void TabsManager::closeChat()
{
	QObject *chat = sender();
        if(chat)
		chat->deleteLater();
}

TabsManager* tabs_manager;
